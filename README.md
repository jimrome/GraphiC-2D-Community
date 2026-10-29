# GraphiC 2D Community Edition (v7.0)

[![CI](https://github.com/jimrome/GraphiC-2D-Community/actions/workflows/ci.yml/badge.svg)](https://github.com/jimrome/GraphiC-2D-Community/actions/workflows/ci.yml)

**GraphiC 2D Community Edition** is a fast, lightweight, standalone 2D scientific plotting library for **C/C++**, **Modern Fortran 2003+**, **Python 3**, **Julia (GraphiC.jl)**, and **Java 22 (Foreign Function & Memory API)**.

Developed in the 1980s by plasma physicists James Rome and George Kelley while working at a national laboratory, GraphiC has generated publication-grade figures for over 40 years. This modernized Community Edition provides native SVG vector output with sub-millisecond execution speeds and zero external rendering dependencies.

---

> [!WARNING]
> ### ⚠️ Important Notice Regarding Example & Test Programs
> The full GraphiC distribution contains dozens of test and example programs. **Programs from the full suite that utilize 3D surfaces (`surmat`), contouring (`conmat`), RF Smith charts (`smdraw`), chemical ternary phase diagrams (`trimat`), polar grids (`polgrid`), or advanced publication typography controls (`tickht`, `titlht`, `upright`, `charspc`, `widen`, `skew`) will not work with the free 2D Community Edition.**
>
> If a routine exclusive to GraphiC Professional is invoked, the library will output an informative notice directing you to the upgrade page.
>
> **Dedicated 2D-only test programs are included in this package** under `c/examples/`, `fortran/examples/`, `python/examples/`, `julia/examples/`, and `java/examples/`.

---

## Edition Feature Comparison

| Feature Area | Free 2D Community Edition | GraphiC Professional 2026 |
| :--- | :---: | :---: |
| **Language Support (C, Fortran, Python, Julia, Java)** | **Yes (Full Parity)** | **Yes (Full Parity)** |
| **2D Cartesian Curves, Polylines & Scatter** | **Yes** | **Yes** |
| **Linear, Logarithmic & Semi-Log Axes** | **Yes** | **Yes** |
| **Vertical Bar Charts & Histograms** | **Yes** | **Yes** |
| **Polygons, Panels & Area Fills** | **Yes** | **Yes** |
| **Error Bars & Vector Direction Fields** | **Yes** | **Yes** |
| **Native SVG Export & Auto-Cropping (`svgcrop`)** | **Yes** | **Yes** |
| **Desktop Auto-Preview (`svgopen`) & Network Push (`svgpush`)** | **Yes** | **Yes** |
| **Outline Hershey Fonts (Swiss & News)** | **Yes** | **Yes** |
| **Native TrueType Engine (`Unicode.ttf`)** | **Yes** | **Yes** |
| **3D Surface Meshes (`surmat`) & Hidden Surface Removal** | ❌ *Pro Only* | **Yes** |
| **2D Elevation Contours (`conmat`) & Color Calibration Keys** | ❌ *Pro Only* | **Yes** |
| **RF Smith Transmission Line Charts (`smdraw`)** | ❌ *Pro Only* | **Yes** |
| **Chemical Ternary Phase Diagrams (`trimat`)** | ❌ *Pro Only* | **Yes** |
| **Polar Coordinate Grids & Curves (`polgrid`)** | ❌ *Pro Only* | **Yes** |
| **16 Hershey Stroke Fonts (Simplex, Complex, Duplex, etc.)** | ❌ *Pro Only* | **Yes** |
| **Fine-Tuning Typography Controls (`tickht`, `titlht`, etc.)** | ❌ *Pro Only* | **Yes** |

To upgrade to **GraphiC Professional 2026** for full 3D, contours, Smith charts, and advanced typography, visit:
**[https://jamesrome.com](https://jamesrome.com)**

---

## Quickstart

### 1. Build the C Library
```bash
make
```
This builds `lib/libgraphic.a` and `lib/libgraphic.dylib` (or `.so`), and compiles the 2D C examples into `bin/`.

### 2. Run C Examples
```bash
./bin/Sample
# Outputs sample.svg - open in any browser or vector editor
open sample.html
```

### 3. Build & Run with Fortran Package Manager (fpm)
Continuously tested via GitHub Actions with `gfortran` (`gcc`) and Intel `ifx` (`icx`) across Linux, macOS, and Windows.

#### A. Consuming GraphiC as an `fpm` Dependency in Your Project
Add `graphic` to your project's `fpm.toml` (see [`fortran/fpm_client/`](fortran/fpm_client/) for a complete standalone client project):
```toml
[dependencies]
graphic = { git = "https://github.com/jimrome/GraphiC-2D-Community.git" }
```
Then `use graphic` in your `app/main.f90` and run `fpm run`:
```fortran
program main
  use graphic
  implicit none
  real :: x(101), y(101)
  integer :: i

  do i = 1, 101
    x(i) = real(i - 1) * 0.1
    y(i) = exp(-0.25 * x(i)) * cos(2.0 * x(i))
  end do

  call gpc_begin(DRIVER_SVG, "damped_oscillator.tkf")
  call gpc_font(1, "Times.ttf", "H")
  call gpc_page(9.0, 6.5)
  call gpc_area2d(6.8, 4.6)
  call gpc_xname("Time t (s)\1")
  call gpc_yname("Amplitude x(t)\1")
  call gpc_head("Damped Harmonic Oscillator\1")
  call gpc_grid(1)
  call gpc_graf(0.0, 2.0, 10.0, -1.0, 0.5, 1.0)
  call gpc_color(COLOR_BLUE)
  call gpc_linethk(5)
  call gpc_curve(x, y, 101, 0)
  call gpc_endplot()
  call gpc_stop()
end program main
```

#### B. Building & Running the Examples in This Repository
```bash
# Build library, Fortran modules, and all 8 examples
fpm build

# Run any example
fpm run --example sample
fpm run --example d2test
fpm run --example logtest
fpm run --example bartest
fpm run --example erftest
fpm run --example multi_y
fpm run --example svgtest
fpm run --example turbulence
```

### 4. Build with Traditional Make (Fortran)
```bash
cd fortran/examples
make
./sample
```

### 5. Run Python Examples
```bash
cd python/examples
python3 sample_2d.py
```

### 6. Run Java 22 Examples
```bash
cd java
mvn clean compile
java --enable-native-access=ALL-UNNAMED -cp target/classes:examples com.sciend.graphic.examples.Sample
```

## Documentation & Reference
- **Comprehensive User Manual (PDF)**: Included in `docs/GraphiC_Modern_Manual.pdf`
- **Markdown Quick Reference**: Included in `docs/MANUAL.md`
- **Online Manual**: [https://jamesrome.com/sites/default/files/GraphiC_Modern_Manual.pdf](https://jamesrome.com/sites/default/files/GraphiC_Modern_Manual.pdf)

---

## Technical Support & Upgrades
- **Website & Purchases**: [https://jamesrome.com](https://jamesrome.com)
- **Copyright**: © 1983–2026 Scientific Endeavors Corporation & Dr. James A. Rome
