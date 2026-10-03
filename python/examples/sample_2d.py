#!/usr/bin/env python3
# ==============================================================================
# GraphiC Python 3 Suite: sample_2d
#
# Prerequisites:
#   Build the native C engine:
#     make
#
# How to Run (from GraphiC repository root):
#   python3 python/examples/sample_2d.py
#
# Output:
#   python/examples/sample_2d.svg
# ==============================================================================
"""
sample_2d.py - 2D Scientific Plotting Example in Python using GraphiC
"""
import math
import sys
import os

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
from graphic import Figure, BLUE, RED, GREEN, BLACK, DASHED, DOTTED, SOLID, SYM_CIRCLE, SYM_SQUARE

def main():
    print("[Python] Running 2D curve example...")
    npts = 80
    x = [i * 0.1 for i in range(npts)]
    y1 = [math.sin(val) * math.exp(-0.15 * val) for val in x]
    y2 = [0.5 * math.cos(val) * math.exp(-0.10 * val) for val in x]

    fig = Figure(width=8.5, height=6.0, bgcolor="white", tkf_name="py_2d_sample.tkf")
    fig.setup_axes(0.0, 1.0, 8.0, -1.0, 0.5, 1.0, "%3.1f", "%3.1f", color=BLACK)
    fig.box(color=BLACK)
    fig.title("Damped Harmonic Oscillators")
    fig.xlabel("Time (seconds)")
    fig.ylabel("Amplitude (arbitrary units)")

    # Plot Curve 1 (solid blue)
    fig.plot(x, y1, color=BLUE, pattern=SOLID)

    # Plot Curve 2 (dashed red with circular markers)
    fig.plot(x, y2, color=RED, pattern=DASHED)
    fig.scatter(x[::5], y2[::5], color=RED, symbol=SYM_CIRCLE)

    output_svg = "python_sample_2d.svg"
    fig.save(output_svg)
    fig.save_html("python_sample_2d.html")
    print(f"[Python] Saved: {output_svg}")

if __name__ == "__main__":
    main()
