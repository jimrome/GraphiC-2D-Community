"""
GraphiC Python Suite: Continuous Color Scale Plots (turbulence)

Demonstrates:
  - Continuous Scientific Colormaps (Viridis)
  - Querying shading color for each individual data point
  - TrueType Font rendering (Arial.ttf)
  - Continuous SVG Gradient Color Bar (colorkey_gradient)
  - 7,850 data points colored continuously by log10 density
"""

import math
import random
import os
import sys

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
from graphic import procedural as gpc
from graphic.constants import (
    COLORMAP_VIRIDIS,
    WHITE,
    BLACK
)

def main():
    random.seed(123456789)
    npts = 7850
    x, y, z = [], [], []
    zmin, zmax = -0.8, 0.8

    for _ in range(npts):
        u = random.random()
        if random.random() < 0.45:
            x_val = 5.0 + 55.0 * (u ** 1.15) + 2.0 * random.gauss(0, 1)
        else:
            x_val = 20.0 + 228.0 * (u ** 1.30) + 2.0 * random.gauss(0, 1)
        if x_val < 5.0: x_val = 5.0
        if x_val > 249.0: x_val = 249.0

        base_y = 0.22 + 0.95 * math.exp(-x_val / 36.0) + 0.18 * math.exp(-x_val / 130.0)
        sigma_y = 0.045 + 0.22 * math.exp(-x_val / 40.0)

        burst = 0.0
        if random.random() < 0.28 * math.exp(-x_val / 32.0):
            ub = random.random()
            if ub < 1e-5: ub = 1e-5
            burst = -0.35 * math.log(ub)

        y_val = base_y + sigma_y * random.gauss(0, 1) + burst
        if y_val < 0.14:
            y_val = 0.14 + 0.06 * math.exp((y_val - 0.14) / 0.06)
        if y_val > 1.58:
            y_val = 1.58 - 0.06 * math.exp((1.58 - y_val) / 0.06)

        norm_y = (y_val - 0.14) / 1.44
        if norm_y < 0.0: norm_y = 0.0
        if norm_y > 1.0: norm_y = 1.0
        temp_factor = math.exp(-x_val / 65.0)
        z_mean = -0.76 + 1.28 * (norm_y ** 0.75) + 0.26 * temp_factor
        z_val = z_mean + 0.06 * random.gauss(0, 1)
        if z_val < zmin: z_val = zmin
        if z_val > zmax: z_val = zmax

        x.append(x_val)
        y.append(y_val)
        z.append(z_val)

    gpc.bgnplot(1, 'S', "turbulence_py.svg")
    gpc.startplot(WHITE)
    gpc.metricunits(0)
    gpc.font("Arial.ttf", 200)
    gpc.fillfont(1)

    gpc.page(9.5, 6.5)
    gpc.physor(1.35, 1.0)
    gpc.area2d(5.8, 4.5)

    gpc.color(BLACK)
    gpc.xnamht(0.16)
    gpc.ynamht(0.16)
    gpc.set_name_space(0, 0.28)
    gpc.set_name_space(1, 0.45)
    gpc.xname("\310Temperature")
    gpc.yname("\310Transport")

    gpc.numht(0.12)
    gpc.graf("%-1.0f", 0.0, 50.0, 250.0, "%-1.1f", 0.0, 0.2, 1.6)

    # Plot continuous scatter points
    gpc.scatter(x, y, z, zmin, zmax, symnum=12, symheight=0.035, colormap=COLORMAP_VIRIDIS)

    # Draw continuous gradient color bar
    gpc.colorkey_gradient(zmin, zmax, colormap=COLORMAP_VIRIDIS,
                          legllx=7.5, leglly=1.0, legurx=7.8, legury=5.5,
                          nticks=8, format="%-3.1f", fontc=chr(200), height=0.11, angle=0)

    # Color bar label
    gpc.prtfnt(8.35, 3.25, "\310log]10@ Density", 0.13, 90)

    gpc.hardcopy('n')
    gpc.endplot()
    gpc.stopplot()
    print("Generated turbulence_py.svg successfully")

if __name__ == "__main__":
    main()
