/*==============================================================================
 * GraphiC Java 22 FFM Suite: Sample
 *
 * Prerequisites:
 *   Build the native C engine and Java SDK:
 *     make
 *     mvn -f java/pom.xml compile
 *
 * How to Compile & Run (from GraphiC repository root):
 *   javac --enable-preview --source 22 -cp java/target/classes -d java/bin java/examples/Sample.java
 *   java --enable-preview --enable-native-access=ALL-UNNAMED -Djava.library.path=lib \
 *        -cp java/target/classes:java/bin examples.Sample
 *
 * Output:
 *   java/examples/sample.svg
 *============================================================================*/

package examples;

import com.sciend.graphic.Color;
import com.sciend.graphic.Figure;
import java.nio.file.Path;

public class Sample {
    public static void main(String[] args) throws Exception {
        int n = 100;
        float[] x = new float[n];
        float[] y1 = new float[n];
        float[] y2 = new float[n];
        for (int i = 0; i < n; i++) {
            x[i] = i * 0.1f;
            y1[i] = (float) Math.sin(x[i]);
            y2[i] = (float) Math.cos(x[i]);
        }

        try (Figure fig = new Figure(8.5f, 6.5f, Color.WHITE, "sample.tkf")) {
            fig.title("Multi-Waveform Analysis (Java Sample)");
            fig.xlabel("Phase (rad)");
            fig.ylabel("Signal Amplitude");
            fig.setupAxes(0.0f, 1.0f, 10.0f, -1.0f, 0.5f, 1.0f, "%3.1f", "%3.1f", Color.BLACK);
            fig.box(Color.BLACK);
            fig.plot(x, y1, Color.BLUE, 0);
            fig.plot(x, y2, Color.RED, 0);
            Path out = Path.of("java/examples/sample.svg");
            fig.save(out);
            System.out.println("[Java] Saved: " + out);
        }
    }
}
