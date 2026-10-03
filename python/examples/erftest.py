#!/usr/bin/env python3
# ==============================================================================
# GraphiC Python 3 Suite: erftest
#
# Prerequisites:
#   Build the native C engine:
#     make
#
# How to Run (from GraphiC repository root):
#   python3 python/examples/erftest.py
#
# Output:
#   python/examples/erftest.svg
# ==============================================================================
"""Error function erf(x) probability curves (corresponds to Erftest.c)."""
import os, sys, math
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
from graphic import Figure

out_svg = os.path.join(os.path.dirname(__file__), "erftest.svg")
n = 100
x = [-3.0 + i * 0.06 for i in range(n)]
y_erf = [math.erf(v) for v in x]
y_gauss = [math.exp(-v * v) / math.sqrt(math.pi) for v in x]

with Figure(8.0, 6.0, background="white", output_path=out_svg) as fig:
    fig.title("Gaussian Error Functions")
    fig.xlabel("x")
    fig.ylabel("Probability")
    fig.setup_axes(-3.0, 1.0, 3.0, -1.0, 0.5, 1.0, "%3.1f", "%3.1f", color="black")
    fig.box(color="black")
    fig.plot(x, y_erf, color="blue", thickness=2)
    fig.plot(x, y_gauss, color="green", thickness=2)
    fig.save(out_svg)
print(f"[Python] Saved: {out_svg}")
