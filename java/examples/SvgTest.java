/*==============================================================================
 * GraphiC Java 22 FFM Suite: SvgTest
 *
 * Demonstrates:
 *   1. Automatic tight SVG bounding-box cropping with svgcrop()
 *   2. HTTP/HTTPS POST network telemetry push with svgpush()
 *   3. Instant desktop preview in default browser/viewer with svgopen()
 *
 * Prerequisites:
 *   Build the native C engine and Java SDK:
 *     make
 *     mvn -f java/pom.xml compile
 *
 * How to Compile & Run (from GraphiC repository root):
 *   javac --enable-preview --release 22 -cp java/target/classes -d java/bin java/examples/SvgTest.java
 *   java --enable-preview --enable-native-access=ALL-UNNAMED -Djava.library.path=lib \
 *        -cp java/target/classes:java/bin examples.SvgTest [--open] [push_url]
 *
 * Output:
 *   java/examples/svgtest.svg (Vector SVG plot)
 *   java/examples/svgtest.html (Interactive Pan/Zoom Viewer)
 *============================================================================*/

package examples;

import com.sciend.graphic.Color;
import com.sciend.graphic.Figure;
import com.sciend.graphic.Graphic;
import com.sciend.graphic.LineStyle;
import java.nio.file.Path;

public class SvgTest {
    public static void main(String[] args) throws Exception {
        System.out.println("=== GraphiC Java 22 (FFM) SVG Push & Auto-Preview Test (SvgTest) ===");

        boolean doOpen = false;
        String pushUrl = null;

        for (String arg : args) {
            if ("--open".equals(arg) || "-o".equals(arg)) {
                doOpen = true;
            } else if (pushUrl == null && !arg.startsWith("-")) {
                pushUrl = arg;
            }
        }

        if (pushUrl == null) {
            pushUrl = System.getenv("GPC_SVG_PUSH_URL");
        }

        int npts = 200;
        float[] x = new float[npts];
        float[] y = new float[npts];

        for (int i = 0; i < npts; i++) {
            x[i] = i * 0.1f;
            y[i] = (float) (Math.exp(-0.15 * x[i]) * Math.sin(2.5 * x[i]));
        }

        Path svgOut = Path.of("java/examples/svgtest.svg");
        Path htmlOut = Path.of("java/examples/svgtest.html");

        try (Figure fig = new Figure(8.5f, 6.0f, Color.WHITE, "svgtest.tkf")) {
            // 1. Enable tight cropping with 0.25 inch margin
            fig.svgcrop(0.25f);

            fig.setupAxes(0.0f, 2.0f, 20.0f, -1.0f, 0.5f, 1.0f, "%g", "%g", Color.BLUE);
            fig.box(Color.BLUE);
            fig.title("Java 22 (FFM) SVG Auto-Crop, Push & Preview Test");
            fig.xlabel("Time (seconds)");
            fig.ylabel("Signal Amplitude (V)");

            fig.plot(x, y, Color.RED, 0);

            fig.save(svgOut);
            fig.saveHtml(htmlOut);
            System.out.println("✔ Plot successfully rendered to '" + svgOut + "' (tightly cropped with svgcrop).");

            // 2. Test svgpush()
            if (pushUrl != null && !pushUrl.isEmpty()) {
                System.out.println("Pushing plot to endpoint '" + pushUrl + "' via svgpush()...");
                int pushRc = fig.svgpush(pushUrl, svgOut.toString());
                if (pushRc == 0) {
                    System.out.println("✔ svgpush() succeeded (HTTP POST transmitted with Content-Type: image/svg+xml).");
                } else {
                    System.out.println("Notice: svgpush() returned status code " + pushRc + ".");
                }
            } else {
                System.out.println("Info: svgpush() test skipped (pass a URL argument or set GPC_SVG_PUSH_URL).");
                System.out.println("      Example: java -cp java/target/classes:java/bin examples.SvgTest http://localhost:8080/live");
            }

            // 3. Test svgopen()
            if (doOpen) {
                System.out.println("Opening '" + svgOut + "' in default viewer with svgopen()...");
                fig.svgopen(svgOut.toString());
                System.out.println("✔ svgopen() dispatched successfully.");
            } else {
                System.out.println("Info: Run with '--open' to launch your default browser/viewer automatically:");
                System.out.println("      Example: java -cp java/target/classes:java/bin examples.SvgTest --open");
            }
        }

        System.out.println("=== Test Completed Successfully ===");
    }
}
