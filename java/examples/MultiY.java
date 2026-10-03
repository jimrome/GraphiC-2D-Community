/*==============================================================================
 * GraphiC Java 22 FFM Suite: MultiY
 *
 * Prerequisites:
 *   Build the native C engine and Java SDK:
 *     make
 *     mvn -f java/pom.xml compile
 *
 * How to Compile & Run (from GraphiC repository root):
 *   javac --enable-preview --source 22 -cp java/target/classes -d java/bin java/examples/MultiY.java
 *   java --enable-preview --enable-native-access=ALL-UNNAMED -Djava.library.path=lib \
 *        -cp java/target/classes:java/bin examples.MultiY
 *
 * Output:
 *   java/examples/multiy.svg
 *============================================================================*/

package examples;

import com.sciend.graphic.Color;
import com.sciend.graphic.Figure;
import java.nio.file.Path;

public class MultiY {
    public static void main(String[] args) throws Exception {
        try (Figure fig = new Figure(9.0f, 7.0f, Color.WHITE, "multi_y.tkf")) {
            fig.simuplot(4);
            // Context 0
            fig.context(0);
            fig.physor(1.5f, 0.8f);
            fig.setupAxes(0.0f, 1.0f, 10.0f, 0.0f, 2.0f, 12.0f, "%-4.1f", "%-4.1f", Color.BLUE);
            fig.pltfnt(2.0f, 6.0f, "Context 1 (Blue Scale)", 0.15f, 0);

            // Context 1
            fig.context(1);
            fig.physor(1.5f, 0.8f);
            fig.orel(-0.35f, 0.0f);
            fig.setupAxes(0.0f, 1.0f, 10.0f, 0.0f, 1.0f, 6.0f, "%-4.1f", "%-4.1f", Color.GREEN);

            // Context 2
            fig.context(2);
            fig.physor(1.5f, 0.8f);
            fig.orel(-0.70f, 0.0f);
            fig.setupAxes(0.0f, 1.0f, 10.0f, 0.0f, 0.5f, 3.0f, "%-4.1f", "%-4.1f", Color.RED);

            fig.simuplot(0);
            Path out = Path.of("java/examples/multi_y.svg");
            fig.save(out);
            System.out.println("[Java] Saved: " + out);
        }
    }
}
