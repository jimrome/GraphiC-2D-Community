#!/usr/bin/env python3
# ==============================================================================
# GraphiC Python 3 Suite: svgtest
#
# Demonstrates:
#   1. Automatic SVG tight cropping with svgcrop()
#   2. HTTP/HTTPS POST network telemetry push with svgpush()
#   3. Automatic browser/viewer preview launch with svgopen()
#
# Prerequisites:
#   Build native C engine:
#     make
#
# How to Run (from GraphiC repository root):
#   python3 python/examples/svgtest.py [--open] [push_url]
#
# Output:
#   python/examples/svgtest.svg (Vector SVG plot)
#   python/examples/svgtest.html (Interactive Pan/Zoom Viewer)
# ==============================================================================
"""SVG auto-crop, HTTP push, and desktop auto-open test."""
import os, sys, math

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import graphic
from graphic import Figure

def main():
    print("=== GraphiC Python 3 SVG Push & Auto-Preview Test (svgtest) ===")

    do_open = False
    push_url = None

    # Parse arguments
    for arg in sys.argv[1:]:
        if arg in ("--open", "-o"):
            do_open = True
        elif push_url is None and not arg.startswith("-"):
            push_url = arg

    if push_url is None:
        push_url = os.environ.get("GPC_SVG_PUSH_URL")

    out_svg = os.path.join(os.path.dirname(__file__), "svgtest.svg")
    n = 200
    x = [i * 0.1 for i in range(n)]
    y = [math.exp(-0.15 * t) * math.sin(2.5 * t) for t in x]

    # Use Figure context manager
    with Figure(8.5, 6.0, background="white", output_path=out_svg) as fig:
        # 1. Enable tight cropping with 0.25 inch margin
        fig.svgcrop(0.25)

        fig.title("Python 3 SVG Auto-Crop, Push & Preview Test")
        fig.xlabel("Time (seconds)")
        fig.ylabel("Signal Amplitude (V)")
        fig.setup_axes(0.0, 2.0, 20.0, -1.0, 0.5, 1.0, "%g", "%g", color="blue")
        fig.box(color="blue")
        fig.plot(x, y, color="red", thickness=2)

        # Plot is completed and flushed when exiting context or calling fig.save()
        fig.save(out_svg)

    print(f"✔ Plot successfully rendered to '{out_svg}' (tightly cropped with svgcrop).")

    # 2. Test svgpush()
    if push_url:
        print(f"Pushing plot to endpoint '{push_url}' via svgpush()...")
        rc = graphic.svgpush(push_url, out_svg)
        if rc == 0:
            print("✔ svgpush() succeeded (HTTP POST transmitted with Content-Type: image/svg+xml).")
        else:
            print(f"Notice: svgpush() returned status code {rc}.")
    else:
        print("Info: svgpush() test skipped (pass a URL argument or set GPC_SVG_PUSH_URL).")
        print("      Example: python3 python/examples/svgtest.py http://localhost:8080/live")

    # 3. Test svgopen()
    if do_open:
        print(f"Opening '{out_svg}' in default viewer with svgopen()...")
        graphic.svgopen(out_svg)
        print("✔ svgopen() dispatched successfully.")
    else:
        print("Info: Run with '--open' to launch your default browser/viewer automatically:")
        print("      Example: python3 python/examples/svgtest.py --open")

    print("=== Test Completed Successfully ===")

if __name__ == "__main__":
    main()
