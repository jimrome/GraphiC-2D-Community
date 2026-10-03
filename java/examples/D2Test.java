/*==============================================================================
 * GraphiC Java 22 FFM Suite: D2Test
 *
 * Prerequisites:
 *   Build the native C engine and Java SDK:
 *     make
 *     mvn -f java/pom.xml compile
 *
 * How to Compile & Run (from GraphiC repository root):
 *   javac --enable-preview --source 22 -cp java/target/classes -d java/bin java/examples/D2Test.java
 *   java --enable-preview --enable-native-access=ALL-UNNAMED -Djava.library.path=lib \
 *        -cp java/target/classes:java/bin examples.D2Test
 *
 * Output:
 *   java/examples/d2test.svg
 *============================================================================*/

package examples;

import com.sciend.graphic.Color;
import com.sciend.graphic.Figure;
import java.nio.file.Path;

public class D2Test {
    public static void main(String[] args) throws Exception {
        int n = 100;
        float[] x = new float[n];
        float[] y1 = new float[n];
        float[] y2 = new float[n];
        for (int i = 0; i < n; i++) {
            x[i] = i * 0.1f;
            y1[i] = (float) Math.sin(x[i]);
            y2[i] = (float) (Math.cos(x[i]) * Math.exp(-0.1 * x[i]));
        }

        try (Figure fig = new Figure(8.0f, 6.0f, Color.WHITE, "d2test.tkf")) {
            fig.title("2D Multi-Curve Scientific Plot (Java D2test)");
            fig.xlabel("Time (seconds)");
            fig.ylabel("Amplitude (volts)");
            fig.setupAxes(0.0f, 1.0f, 10.0f, -1.2f, 0.4f, 1.2f, "%3.1f", "%3.1f", Color.BLACK);
            fig.box(Color.BLACK);
            fig.plot(x, y1, Color.BLUE, 0);
            fig.plot(x, y2, Color.RED, 0);

            float[] xs = new float[10];
            float[] ys = new float[10];
            for (int i = 0; i < 10; i++) {
                xs[i] = x[i * 10];
                ys[i] = y1[i * 10];
            }
            fig.scatter(xs, ys, Color.BLUE, 1, 1);

            Path out = Path.of("java/examples/d2test.svg");
            fig.save(out);
            System.out.println("[Java] Saved: " + out);
        }
    }
}
