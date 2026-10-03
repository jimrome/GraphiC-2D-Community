/*==============================================================================
 * GraphiC Java 22 FFM Suite: LogTest
 *
 * Prerequisites:
 *   Build the native C engine and Java SDK:
 *     make
 *     mvn -f java/pom.xml compile
 *
 * How to Compile & Run (from GraphiC repository root):
 *   javac --enable-preview --source 22 -cp java/target/classes -d java/bin java/examples/LogTest.java
 *   java --enable-preview --enable-native-access=ALL-UNNAMED -Djava.library.path=lib \
 *        -cp java/target/classes:java/bin examples.LogTest
 *
 * Output:
 *   java/examples/logtest.svg
 *============================================================================*/

package examples;

import com.sciend.graphic.Color;
import com.sciend.graphic.Figure;
import java.nio.file.Path;

public class LogTest {
    public static void main(String[] args) throws Exception {
        int npts = 200;
        float[] x = new float[npts];
        float[] y = new float[npts];
        for (int i = 0; i < npts; i++) {
            x[i] = 0.1f + i * 0.5f;
            y[i] = (float) (Math.sin(x[i]) / (1.0 + 0.1 * x[i]));
        }

        try (Figure fig = new Figure(8.0f, 6.0f, Color.WHITE, "logtest.tkf")) {
            fig.title("Semi-Log Frequency Response (Java Logtest)");
            fig.xlabel("Frequency (kHz, log)");
            fig.ylabel("Gain (dB)");
            fig.xlog(0.1f, 100.0f, "%-1.1f", -1.0f, 0.4f, 1.0f);
            fig.box(Color.BLACK);
            fig.plot(x, y, Color.BLUE, 0);
            Path out = Path.of("java/examples/logtest.svg");
            fig.save(out);
            System.out.println("[Java] Saved: " + out);
        }
    }
}
