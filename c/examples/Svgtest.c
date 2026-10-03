/*==============================================================================
 * GraphiC Modern Test Program: Svgtest
 *
 * Demonstrates:
 *   1. Automatic tight SVG bounding-box cropping with svgcrop()
 *   2. Instant desktop preview in default browser/viewer with svgopen()
 *   3. Network telemetry push via HTTP/HTTPS POST with svgpush()
 *
 * How to Compile & Run (from GraphiC repository root):
 *   make -j4
 *   clang -O2 -IInclude -DGPC_PORTABLE=1 Examples/Svgtest.c lib/libgraphic.a -lm -o bin/Svgtest
 *   ./bin/Svgtest [--open] [push_url]
 *============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <graphic.h>

int main(int argc, char *argv[])
{
    float *x, *y;
    int i, npts = 200;
    int do_open = 0;
    char *push_url = NULL;

    /* Parse command-line flags */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--open") == 0 || strcmp(argv[i], "-o") == 0) {
            do_open = 1;
        } else if (push_url == NULL && argv[i][0] != '-') {
            push_url = argv[i];
        }
    }

    printf("=== GraphiC SVG Push & Auto-Preview Test (Svgtest) ===\n");

    /* Allocate data points for a damped sinusoidal curve */
    x = (float *)gpcalloc(npts, sizeof(float));
    y = (float *)gpcalloc(npts, sizeof(float));
    if (x == NULL || y == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for plot data.\n");
        return 1;
    }

    for (i = 0; i < npts; i++) {
        x[i] = (float)i * 0.1f;
        y[i] = (float)(exp(-0.15 * x[i]) * sin(2.5 * x[i]));
    }

    /* Begin plot session: emits svgtest.svg and companion svgtest.html */
    bgnplot(1, 'g', "svgtest.tkf");
    startplot(WHITE);

    /* 1. Set auto-crop margin to 1/4 inch */
    svgcrop(0.25f);

    /* Page and coordinate setup */
    page(8.5f, 6.0f);
    area2d(7.0f, 4.5f);

    font(1, "Swiss.fnt", '\310');
    heading("\310GraphiC 2026: SVG Auto-Crop, Push & Preview Test");
    xname("\310Time (seconds)");
    yname("\310Signal Amplitude (V)");

    color(BLUE);
    graf("%g", 0.0f, 2.0f, 20.0f, "%g", -1.0f, 0.5f, 1.0f);
    grid(1);

    color(RED);
    curve(x, y, npts, 0);

    /* Close the plot (hardcopy('n') allows non-blocking execution) */
    hardcopy('n');
    endplot();
    printf("✔ Plot successfully rendered to 'svgtest.svg' (tightly cropped with svgcrop).\n");

    /* 2. Test svgpush() if URL specified or configured in environment */
    if (push_url != NULL || getenv("GPC_SVG_PUSH_URL") != NULL) {
        const char *target = push_url ? push_url : getenv("GPC_SVG_PUSH_URL");
        printf("Pushing plot to endpoint '%s' via svgpush()...\n", target);
        int push_rc = svgpush(push_url, "svgtest.svg");
        if (push_rc == 0) {
            printf("✔ svgpush() succeeded (HTTP POST transmitted with Content-Type: image/svg+xml).\n");
        } else {
            printf("Notice: svgpush() returned status code %d.\n", push_rc);
        }
    } else {
        printf("Info: svgpush() test skipped (pass a URL argument or set GPC_SVG_PUSH_URL to push).\n");
        printf("      Example: ./bin/Svgtest http://localhost:8080/live\n");
    }

    /* 3. Test svgopen() */
    if (do_open) {
        printf("Opening 'svgtest.svg' in default system viewer/browser with svgopen()...\n");
        svgopen("svgtest.svg");
        printf("✔ svgopen() dispatched successfully.\n");
    } else {
        printf("Info: Run with '--open' to launch your default browser/viewer automatically:\n");
        printf("      Example: ./bin/Svgtest --open\n");
    }

    /* Free dynamically allocated memory */
    gpcfree((void **)&x);
    gpcfree((void **)&y);

    printf("=== Test Completed Successfully ===\n");
    return 0;
}
