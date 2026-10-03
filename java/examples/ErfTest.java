/*==============================================================================
 * GraphiC Java 22 FFM Suite: ErfTest
 *
 * Prerequisites:
 *   Build the native C engine and Java SDK:
 *     make
 *     mvn -f java/pom.xml compile
 *
 * How to Compile & Run (from GraphiC repository root):
 *   javac --enable-preview --source 22 -cp java/target/classes -d java/bin java/examples/ErfTest.java
 *   java --enable-preview --enable-native-access=ALL-UNNAMED -Djava.library.path=lib \
 *        -cp java/target/classes:java/bin examples.ErfTest
 *
 * Output:
 *   java/examples/erftest.svg
 *============================================================================*/

package examples;

import com.sciend.graphic.Color;
import com.sciend.graphic.Figure;
import java.nio.file.Path;

public class ErfTest {
    public static void main(String[] args) throws Exception {
        int n = 100;
        float[] x = new float[n];
        float[] yGauss = new float[n];
        for (int i = 0; i < n; i++) {
            x[i] = -3.0f + i * 0.06f;
            yGauss[i] = (float) (Math.exp(-x[i] * x[i]) / Math.sqrt(Math.PI));
        }

        try (Figure fig = new Figure(8.0f, 6.0f, Color.WHITE, "erftest.tkf")) {
            fig.title("Gaussian Probability Distribution (Java Erftest)");
            fig.xlabel("x");
            fig.ylabel("Probability Density");
            fig.setupAxes(-3.0f, 1.0f, 3.0f, 0.0f, 0.2f, 0.8f, "%3.1f", "%3.1f", Color.BLACK);
            fig.box(Color.BLACK);
            fig.plot(x, yGauss, Color.GREEN, 0);
            Path out = Path.of("java/examples/erftest.svg");
            fig.save(out);
            System.out.println("[Java] Saved: " + out);
        }
    }
}
