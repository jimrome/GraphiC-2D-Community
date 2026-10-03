#!/usr/bin/env python3
# ==============================================================================
# GraphiC Python 3 Suite: multi_y
#
# Prerequisites:
#   Build the native C engine:
#     make
#
# How to Run (from GraphiC repository root):
#   python3 python/examples/multi_y.py
#
# Output:
#   python/examples/multi_y.svg
# ==============================================================================
"""Multiple independent Y axes (corresponds to Multi_y.c)."""
import os, sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
from graphic import Figure

out_svg = os.path.join(os.path.dirname(__file__), "multi_y.svg")

with Figure(9.0, 7.0, background="white", output_path=out_svg) as fig:
    fig.simuplot(4)
    fig.context(0)
    fig.physor(1.5, 0.8)
    fig.setup_axes(0.0, 1.0, 10.0, 0.0, 2.0, 12.0, "%-4.1f", "%-4.1f", color="blue")
    fig.pltfnt(2.0, 6.0, "Context 1 (Blue Scale)", 0.15, 0)
    fig.context(1)
    fig.physor(1.5, 0.8)
    fig.orel(-0.35, 0.0)
    fig.setup_axes(0.0, 1.0, 10.0, 0.0, 1.0, 6.0, "%-4.1f", "%-4.1f", color="green")
    fig.context(2)
    fig.physor(1.5, 0.8)
    fig.orel(-0.70, 0.0)
    fig.setup_axes(0.0, 1.0, 10.0, 0.0, 0.5, 3.0, "%-4.1f", "%-4.1f", color="red")
    fig.simuplot(0)
    fig.save(out_svg)
print(f"[Python] Saved: {out_svg}")
