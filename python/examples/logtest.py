#!/usr/bin/env python3
# ==============================================================================
# GraphiC Python 3 Suite: logtest
#
# Prerequisites:
#   Build the native C engine:
#     make
#
# How to Run (from GraphiC repository root):
#   python3 python/examples/logtest.py
#
# Output:
#   python/examples/logtest.svg
# ==============================================================================
"""Semi-log frequency response (corresponds to Logtest.c)."""
import os, sys, math
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
from graphic import Figure

out_svg = os.path.join(os.path.dirname(__file__), "logtest.svg")
npts = 200
x = [0.1 + i * 0.5 for i in range(npts)]
y = [math.sin(v) / (1.0 + 0.1 * v) for v in x]

with Figure(8.0, 6.0, background="white", output_path=out_svg) as fig:
    fig.title("Semi-Log Frequency Response (Logtest)")
    fig.xlabel("Frequency (kHz, log)")
    fig.ylabel("Gain (dB)")
    fig.xlog(0.1, 100.0, "%-1.1f", -1.0, 0.4, 1.0)
    fig.box(color="black")
    fig.plot(x, y, color="blue", thickness=2)
    fig.save(out_svg)
print(f"[Python] Saved: {out_svg}")
