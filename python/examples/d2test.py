#!/usr/bin/env python3
# ==============================================================================
# GraphiC Python 3 Suite: d2test
#
# Prerequisites:
#   Build the native C engine:
#     make
#
# How to Run (from GraphiC repository root):
#   python3 python/examples/d2test.py
#
# Output:
#   python/examples/d2test.svg
# ==============================================================================
"""2D Linear multi-curve plot with markers (corresponds to D2test.c)."""
import os, sys, math
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
from graphic import Figure

out_svg = os.path.join(os.path.dirname(__file__), "d2test.svg")
n = 100
x = [i * 0.1 for i in range(n)]
y1 = [math.sin(v) for v in x]
y2 = [math.cos(v) * math.exp(-0.1 * v) for v in x]

with Figure(8.0, 6.0, background="white", output_path=out_svg) as fig:
    fig.title("2D Multi-Curve Scientific Plot (D2test)")
    fig.xlabel("Time (seconds)")
    fig.ylabel("Amplitude (volts)")
    fig.setup_axes(0.0, 1.0, 10.0, -1.2, 0.4, 1.2, "%3.1f", "%3.1f", color="black")
    fig.box(color="black")
    fig.plot(x, y1, color="blue", thickness=2)
    fig.plot(x, y2, color="red", thickness=2)
    xs = [x[i] for i in range(0, n, 10)]
    ys = [y1[i] for i in range(0, n, 10)]
    fig.scatter(xs, ys, symbol_id=1, color="dark_blue")
    fig.save(out_svg)
print(f"[Python] Saved: {out_svg}")
