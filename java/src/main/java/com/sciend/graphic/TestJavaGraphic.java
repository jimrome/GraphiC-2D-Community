package com.sciend.graphic;

public class TestJavaGraphic {

    public static void main(String[] args) throws Exception {
        System.out.println("=== Testing GraphiC Java 22 FFM Bindings ===");

        test2DPlot();
        test3DSurface();
        testSmithChart();

        System.out.println("\nALL JAVA GRAPHIC TESTS PASSED SUCCESSFULLY!");
    }

    private static void test2DPlot() throws Exception {
        System.out.println("Testing 2D Curve & Scatter Plot in Java...");

        int n = 100;
        float[] x = new float[n];
        float[] y1 = new float[n];
        float[] y2 = new float[n];

        for (int i = 0; i < n; i++) {
            x[i] = i * 0.1f;
            y1[i] = (float) Math.sin(x[i]);
            y2[i] = (float) Math.cos(x[i]);
        }

        try (Figure fig = new Figure(8.0f, 6.0f, Color.WHITE, "java_2d.tkf")) {
            fig.setupAxes(0f, 2f, 10f, -1.5f, 0.5f, 1.5f);
            fig.title("Java 22 FFM: Harmonic Oscillation");
            fig.xlabel("Time (seconds)");
            fig.ylabel("Amplitude (Volts)");

            fig.plot(x, y1, Color.BLUE, LineStyle.SOLID);
            fig.plot(x, y2, Color.RED, LineStyle.DASHED);

            float[] xPts = new float[10];
            float[] yPts = new float[10];
            for (int i = 0; i < 10; i++) {
                xPts[i] = x[i * 10];
                yPts[i] = y1[i * 10];
            }
            fig.scatter(xPts, yPts, Color.GREEN, Symbol.CIRCLE);

            fig.save("java_2d.svg");
            System.out.println("  Saved java_2d.svg");
        }
    }

    private static void test3DSurface() throws Exception {
        System.out.println("Testing 3D Surface Plot in Java...");

        int nx = 25;
        int ny = 25;
        float[] z = new float[nx * ny];

        float pi = (float) Math.PI;
        for (int j = 0; j < ny; j++) {
            float y = j * (pi / (ny - 1));
            for (int i = 0; i < nx; i++) {
                float x = -pi + i * (2 * pi / (nx - 1));
                z[j * nx + i] = (float) (Math.sin(y) * Math.cos(x - y));
            }
        }

        try (Figure fig = new Figure(8.5f, 6.5f, Color.WHITE, "java_3d.tkf")) {
            fig.plotSurface(
                    z, nx, ny,
                    new float[]{-pi, 1.0f, pi},
                    new float[]{0.0f, 1.0f, pi},
                    new float[]{-1.0f, 0.5f, 1.0f},
                    Color.BROWN, Color.MAGENTA,
                    "Java 3D Surface Mesh: sin(y)*cos(x-y)"
            );

            fig.save("java_3d.svg");
            System.out.println("  Saved java_3d.svg");
        }
    }

    private static void testSmithChart() throws Exception {
        System.out.println("Testing Smith Chart in Java...");

        try (Figure fig = new Figure(7.0f, 7.0f, Color.WHITE, "java_smith.tkf")) {
            fig.title("Java RF Smith Chart");
            float[][] traces = new float[][]{
                    {0.5f, 0.5f},
                    {1.0f, 0.0f},
                    {2.0f, -1.0f}
            };
            fig.smithChart(Color.YELLOW, traces);

            fig.save("java_smith.svg");
            System.out.println("  Saved java_smith.svg");
        }
    }
}
