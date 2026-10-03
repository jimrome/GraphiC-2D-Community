/*==============================================================================
 * GraphiC Classic C Suite: Logtest
 *
 * How to Compile & Run (from GraphiC repository root):
 *   make examples
 *   ./bin/Logtest
 *
 * Direct Command-Line Compilation:
 *   clang -O2 -IInclude -DGPC_PORTABLE=1 c/examples/Logtest.c lib/libgraphic.a -lm -o bin/Logtest
 *   ./bin/Logtest
 *============================================================================*/

#include "graphic.h"
#include <math.h>

int main(void) {
    int npts = 100;
    float freq[100], gain[100];
    int i;

    /* Compute 2-pole Butterworth low-pass filter frequency response (cutoff 1 kHz) */
    for (i = 0; i < npts; i++) {
        float f = powf(10.0f, 0.0f + (float)i * 5.0f / (float)(npts - 1)); /* 1.0 to 1.0e5 Hz */
        float w = f / 1000.0f;
        float h = 1.0f / sqrtf(1.0f + w * w * w * w);
        freq[i] = f;
        gain[i] = 20.0f * log10f(h);
    }

    bgnplot(0, 'S', "c/examples/logtest.svg");
    page(8.0f, 6.0f);
    startplot(WHITE);
    area2d(5.5f, 4.0f);
    physor(1.5f, 1.2f);
    box();
    heading("Bode Magnitude Plot");
    xname("Frequency (Hz)");
    yname("Gain (dB)");
    grid(1);
    upright(1);
    xlog(1.0f, 1.0e5f, "%2.0f", -40.0f, 10.0f, 20.0f);
    color(BLUE);
    tcurve(3);
    curve(freq, gain, npts, 0);
    tcurve(0);
    endplot();
    stopplot();
    return 0;
}
