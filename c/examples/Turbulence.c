/*==============================================================================
 * GraphiC Classic C Suite: Continuous Color Scale Plots (turbulence)
 *
 * Demonstrates:
 *   - Continuous Scientific Colormaps (Viridis)
 *   - Querying shading color for each individual data point
 *   - TrueType Font rendering (Arial.ttf)
 *   - Continuous SVG Gradient Color Bar (ColorKeyGradient)
 *   - 7,850 data points colored continuously by log10 density
 *============================================================================*/

#include <graphic.h>
#include <colormap.h>
#include <math.h>

#define NPTS 7850

/* Deterministic pseudo-random generator */
static unsigned long seed = 123456789UL;
static float rand_uniform(void) {
    seed = (seed * 1103515245UL + 12345UL) & 0x7fffffffUL;
    return (float)seed / (float)0x7fffffffUL;
}

static float rand_gauss(void) {
    float u1 = rand_uniform();
    float u2 = rand_uniform();
    if (u1 < 1e-7f) u1 = 1e-7f;
    return sqrtf(-2.0f * logf(u1)) * cosf(6.2831853f * u2);
}

int GPC_MAIN(void)
{
    float x[NPTS], y[NPTS], z[NPTS];
    float zmin = -0.8f, zmax = 0.8f;
    int i;

    /* Generate 7,850 continuous synthetic data points with smooth physical decay */
    for (i = 0; i < NPTS; i++) {
        float u = rand_uniform();
        float x_val;
        float base_y, sigma_y, burst, y_val;
        float norm_y, temp_factor, z_mean, z_val;

        /* Continuous temperature sampling across [5, 250] */
        if (rand_uniform() < 0.45f) {
            x_val = 5.0f + 55.0f * powf(u, 1.15f) + 2.0f * rand_gauss();
        } else {
            x_val = 20.0f + 228.0f * powf(u, 1.30f) + 2.0f * rand_gauss();
        }
        if (x_val < 5.0f) x_val = 5.0f;
        if (x_val > 249.0f) x_val = 249.0f;

        /* Smooth transport decay with natural fluctuation */
        base_y = 0.22f + 0.95f * expf(-x_val / 36.0f) + 0.18f * expf(-x_val / 130.0f);
        sigma_y = 0.045f + 0.22f * expf(-x_val / 40.0f);

        burst = 0.0f;
        if (rand_uniform() < 0.28f * expf(-x_val / 32.0f)) {
            float ub = rand_uniform();
            if (ub < 1e-5f) ub = 1e-5f;
            burst = -0.35f * logf(ub);
        }

        y_val = base_y + sigma_y * rand_gauss() + burst;
        /* Smooth soft boundary to avoid artificial flat floors */
        if (y_val < 0.14f) {
            y_val = 0.14f + 0.06f * expf((y_val - 0.14f) / 0.06f);
        }
        if (y_val > 1.58f) {
            y_val = 1.58f - 0.06f * expf((1.58f - y_val) / 0.06f);
        }

        /* Color scale: log10 Density smoothly correlated with transport and temperature */
        norm_y = (y_val - 0.14f) / 1.44f;
        if (norm_y < 0.0f) norm_y = 0.0f;
        if (norm_y > 1.0f) norm_y = 1.0f;
        temp_factor = expf(-x_val / 65.0f);
        z_mean = -0.76f + 1.28f * powf(norm_y, 0.75f) + 0.26f * temp_factor;
        z_val = z_mean + 0.06f * rand_gauss();
        if (z_val < zmin) z_val = zmin;
        if (z_val > zmax) z_val = zmax;

        x[i] = x_val;
        y[i] = y_val;
        z[i] = z_val;
    }

    /* Start plot session */
    bgnplot(1, 'S', "turbulence.svg");
    startplot(WHITE);
    metricunits(0);

    /* Load TrueType font Arial.ttf */
    font(1, "Arial.ttf", '\310');
    fillfont(1);

    page(9.5f, 6.5f);
    physor(1.35f, 1.0f);
    area2d(5.8f, 4.5f);

    /* Anonymized axis labels without top caption */
    color(BLACK);
    xnamht(0.16f);
    ynamht(0.16f);
    SetNameSpace(0, 0.28f);
    SetNameSpace(1, 0.45f);
    xname("\310Temperature");
    yname("\310Transport");

    /* Cartesian grid and axes */
    numht(0.12f);
    graf("%-1.0f", 0.0f, 50.0f, 250.0f, "%-1.1f", 0.0f, 0.2f, 1.6f);

    /* Plot all 7,850 points colored continuously via Viridis colormap */
    gpc_scatter(x, y, z, NPTS, zmin, zmax, 12, 0.035f, GPC_COLORMAP_VIRIDIS);

    /* Draw continuous SVG Gradient Color Bar on the right */
    ColorKeyGradient(zmin, zmax, GPC_COLORMAP_VIRIDIS,
                     7.5f, 1.0f, 7.8f, 5.5f,
                     8, "%-3.1f", '\310', 0.11f, 0);

    /* Color bar label */
    prtfnt(8.35f, 3.25f, "\310log]10@ Density", 0.13f, 90);

    /* Close plot without interactive pause */
    hardcopy('n');
    endplot();
    stopplot();
    return 0;
}
