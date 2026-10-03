#!/usr/bin/env python3
# ==============================================================================
# GraphiC Python 3 Suite: bartest
#
# Prerequisites:
#   Build the native C engine:
#     make
#
# How to Run (from GraphiC repository root):
#   python3 python/examples/bartest.py
#
# Output:
#   python/examples/bartest.svg
# ==============================================================================
"""2D Grouped bar chart with patterns (corresponds to Bartest.c)."""
import os, sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
from graphic import Figure

out_svg = os.path.join(os.path.dirname(__file__), "bartest.svg")
x_bars = [1.0, 2.0, 3.0, 4.0]
y_bars = [12.5, 18.2, 15.0, 22.8]
colors = ["blue", "green", "red", "black"]
patterns = [1, 2, 5, 6]

with Figure(8.0, 6.0, background="white", output_path=out_svg) as fig:
    fig.title("Quarterly Revenue by Division (Bartest)")
    fig.xlabel("Quarter")
    fig.ylabel("Revenue ($ Millions)")
    fig.setup_axes(0.0, 1.0, 5.0, 0.0, 5.0, 25.0, "%g", "%g", color="black")
    fig.box(color="black")
    fig.bar(x_bars, y_bars, nxdiv=5, bwid=0.6, mode=0, colors=colors, patterns=patterns)
    fig.save(out_svg)
    fig.save_html(out_svg.replace(".svg", ".html"))
print(f"[Python] Saved: {out_svg}")
