/*==============================================================================
 * GraphiC Java 22 FFM Suite: BarTest
 *
 * Prerequisites:
 *   Build the native C engine and Java SDK:
 *     make
 *     mvn -f java/pom.xml compile
 *
 * How to Compile & Run (from GraphiC repository root):
 *   javac --enable-preview --source 22 -cp java/target/classes -d java/bin java/examples/BarTest.java
 *   java --enable-preview --enable-native-access=ALL-UNNAMED -Djava.library.path=lib \
 *        -cp java/target/classes:java/bin examples.BarTest
 *
 * Output:
 *   java/examples/bartest.svg
 *============================================================================*/

package examples;

import com.sciend.graphic.Color;
import com.sciend.graphic.Figure;
import java.nio.file.Path;

public class BarTest {
    public static void main(String[] args) throws Exception {
        float[] xBars = new float[]{1.0f, 2.0f, 3.0f, 4.0f};
        float[] yBars = new float[]{12.5f, 18.2f, 15.0f, 22.8f};
        Color[] colors = new Color[]{Color.BLUE, Color.GREEN, Color.RED, Color.BLACK};
        int[] patterns = new int[]{1, 2, 5, 6};

        try (Figure fig = new Figure(8.0f, 6.0f, Color.WHITE, "bartest.tkf")) {
            fig.title("Quarterly Department Revenue (Java Bartest)");
            fig.xlabel("Quarter");
            fig.ylabel("Revenue ($ Millions)");
            fig.setupAxes(0.0f, 1.0f, 5.0f, 0.0f, 5.0f, 25.0f, "%g", "%g", Color.BLACK);
            fig.box(Color.BLACK);
            fig.bar(xBars, yBars, 5, 0.6f, 0, colors, patterns);
            Path out = Path.of("java/examples/bartest.svg");
            fig.save(out);
            fig.saveHtml(Path.of("java/examples/bartest.html"));
            System.out.println("[Java] Saved: " + out);
        }
    }
}
