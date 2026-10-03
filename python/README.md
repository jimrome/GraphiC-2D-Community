# GraphiC for Python

Modern Python bindings to the **GraphiC** publication-quality scientific visualization engine.

## Features
- **Native Vector SVG Output**: Generates pure, scalable SVG with interactive pan/zoom HTML5 companion.
- **Jupyter Notebook Integration**: Rich `_repr_svg_()` support automatically displays plots in Jupyter notebooks.
- **Scientific Plot Types**:
  - 2D curves, multi-line plots, symbol scatters, linear/log axes
  - 3D surface meshes with hidden line removal and perspective viewpoint controls
  - 2D contour maps with labeled level curves
  - RF Smith charts with impedance circles and admittance curves
  - Ternary mixture diagrams
- **Zero Heavy Dependencies**: Built on standard library `ctypes`. Seamlessly accepts standard Python lists or `numpy` arrays.

## Quickstart

```python
import math
from graphic import Figure, BLUE, RED, DASHED

# 1. Generate sample data
x = [i * 0.1 for i in range(100)]
y1 = [math.sin(val) for val in x]
y2 = [math.cos(val) for val in x]

# 2. Plot with context manager
with Figure(width=8.0, height=5.5) as fig:
    fig.setup_axes(0, 2, 10, -1.5, 0.5, 1.5)
    fig.title("Harmonic Oscillator Response")
    fig.xlabel("Time (seconds)")
    fig.ylabel("Amplitude")

    fig.plot(x, y1, color="blue", label="Sin Wave")
    fig.plot(x, y2, color="red", pattern="dashed", label="Cos Wave")

    fig.save("oscillator.svg")
```
