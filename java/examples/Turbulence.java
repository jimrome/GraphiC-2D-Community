package examples;

import com.sciend.graphic.Graphic;
import java.util.Random;

public class Turbulence {
    public static void main(String[] args) {
        Random rand = new Random(123456789L);
        int npts = 7850;
        float[] x = new float[npts];
        float[] y = new float[npts];
        float[] z = new float[npts];
        float zmin = -0.8f, zmax = 0.8f;

        for (int i = 0; i < npts; i++) {
            float u = rand.nextFloat();
            float xVal;
            if (rand.nextFloat() < 0.45f) {
                xVal = 5.0f + 55.0f * (float) Math.pow(u, 1.15) + 2.0f * (float) rand.nextGaussian();
            } else {
                xVal = 20.0f + 228.0f * (float) Math.pow(u, 1.30) + 2.0f * (float) rand.nextGaussian();
            }
            if (xVal < 5.0f) xVal = 5.0f;
            if (xVal > 249.0f) xVal = 249.0f;

            float baseY = 0.22f + 0.95f * (float) Math.exp(-xVal / 36.0f) + 0.18f * (float) Math.exp(-xVal / 130.0f);
            float sigmaY = 0.045f + 0.22f * (float) Math.exp(-xVal / 40.0f);

            float burst = 0.0f;
            if (rand.nextFloat() < 0.28f * (float) Math.exp(-xVal / 32.0f)) {
                float ub = rand.nextFloat();
                if (ub < 1e-5f) ub = 1e-5f;
                burst = -0.35f * (float) Math.log(ub);
            }

            float yVal = baseY + sigmaY * (float) rand.nextGaussian() + burst;
            if (yVal < 0.14f) {
                yVal = 0.14f + 0.06f * (float) Math.exp((yVal - 0.14f) / 0.06f);
            }
            if (yVal > 1.58f) {
                yVal = 1.58f - 0.06f * (float) Math.exp((1.58f - yVal) / 0.06f);
            }

            float normY = (yVal - 0.14f) / 1.44f;
            if (normY < 0.0f) normY = 0.0f;
            if (normY > 1.0f) normY = 1.0f;
            float tempFactor = (float) Math.exp(-xVal / 65.0f);
            float zMean = -0.76f + 1.28f * (float) Math.pow(normY, 0.75) + 0.26f * tempFactor;
            float zVal = zMean + 0.06f * (float) rand.nextGaussian();
            if (zVal < zmin) zVal = zmin;
            if (zVal > zmax) zVal = zmax;

            x[i] = xVal;
            y[i] = yVal;
            z[i] = zVal;
        }

        Graphic.bgnplot(1, 'S', "turbulence_java.svg");
        Graphic.startplot(Graphic.WHITE);
        Graphic.metricunits(0);
        Graphic.font("Arial.ttf", (byte) 200);
        Graphic.fillfont(1);

        Graphic.page(9.5f, 6.5f);
        Graphic.physor(1.35f, 1.0f);
        Graphic.area2d(5.8f, 4.5f);

        Graphic.color(Graphic.BLACK);
        Graphic.setNameSpace(0, 0.28f);
        Graphic.setNameSpace(1, 0.45f);
        Graphic.xname("\310Temperature");
        Graphic.yname("\310Transport");

        Graphic.graf("%-1.0f", 0.0f, 50.0f, 250.0f, "%-1.1f", 0.0f, 0.2f, 1.6f);

        // Continuous colored scatter points via Viridis colormap
        Graphic.scatter(x, y, z, zmin, zmax, 12, 0.035f, Graphic.COLORMAP_VIRIDIS);

        // Continuous SVG Gradient Color Bar
        Graphic.colorkeyGradient(zmin, zmax, Graphic.COLORMAP_VIRIDIS,
                                7.5f, 1.0f, 7.8f, 5.5f,
                                8, "%-3.1f", (char) 200, 0.11f, 0);

        // Color bar label
        Graphic.prtfnt(8.35f, 3.25f, "\310log]10@ Density", 0.13f, 90);

        Graphic.hardcopy((byte) 'n');
        Graphic.endplot();
        Graphic.stopplot();
        System.out.println("Generated turbulence_java.svg successfully");
    }
}
