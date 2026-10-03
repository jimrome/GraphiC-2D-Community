# GraphiC Changelog

All notable changes to the GraphiC scientific graphics library starting from the modern release date (September 7, 2026) are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [7.3.0] - 2026-09-30

### Added
- **Unified Standalone Packaging Architecture (`package_distribution.py`, `package_community.py`)**:
  - Eliminated external Git dependency friction by adopting a 100% standalone downloadable ZIP archive model for both Community and Professional editions.
  - **GraphiC Professional (`GraphiC-2026-v7.3.0.zip`)**: Delivered as a complete full-source commercial archive exclusively via **Lemon Squeezy** (with legacy mirror `GraphiC-2026.zip`), strictly isolating proprietary 3D surface, contouring, Smith chart, and ternary phase routines.
  - **GraphiC 2D Community Edition (`GraphiC-2D-Community-v7.3.0.zip`)**: Standalone archive containing 39 2D C sources, `community_stubs.c`, Hershey outline fonts, TrueType engine, and curated 2D examples across C, Fortran, Python, and Java (mirrored to `GraphiC-2D-Community.zip` and `GraphiC_2D_Community.zip` for existing permalinks).
  - **Unified Fortran Package Manager (`fpm`) Workflow**: Both Community and Professional editions are consumed with an identical local path configuration (`graphic = { path = "../GraphiC" }`), removing Git access tokens, network delays, and permission management.
  - **Clean Distribution Policy**: Release archives package solely the vector manual PDF (`GraphiC_Modern_Manual.pdf`) in `docs/`, stripping Word `.docx` and Markdown source documents.
- **Publication-Grade Typography Architecture Diagram (Figure 5.1)**:
  - Replaced the ASCII text block in Chapter 5 with a publication-grade vector SVG (`docs/images/typography_architecture.svg`) and 300-DPI PNG fallback (`Figure 5.1 — GraphiC Multi-Engine Typography Architecture`).
  - Illustrates the three parallel typography subsystems: Vector Stroke Fonts (`.FNT` - Hershey primitives), TrueType Engine (`.TTF` - Quadratic Bézier B-splines), and PostScript Type 1 (`.PFB` - Cubic Bézier CharStrings).
- **Dedicated 3D Visualization Documentation & Polyglot Bindings**:
  - Replaced the Section 7.10 3D stub with three dedicated reference cards featuring complete parameter tables, descriptions, and 4-language signatures:
    - `d3bars(bwid)`: Level 2 [GraphiC Professional] perspective column bars.
    - `plane3d(wplane, pos, hide)`: Level 2 [GraphiC Professional] modernized 3-argument planar cross-section routine.
    - `WaterfallPlot(x, y, z, nx, ny)` & `WaterfallCurve(x, y, z, npts)`: Level 2 [GraphiC Professional] spectral waterfall plots.
  - Implemented and verified native bindings for `WaterfallCurve` and `plane3d` across Modern Fortran (`graphic_iso.f90`, `graphic.f90`), Python 3 (`_bindings.py`, `procedural.py`, `figure.py`), and Java 22 FFM (`GraphicNative.java`, `Graphic.java`, `Figure.java`).
- **Comprehensive Toolchain, Compiler & `fpm` Documentation (§3.5)**:
  - Documented supported compilers across Fortran (GNU `gfortran` 4.8+, Intel `ifx`/`ifort`, LLVM `flang`, NVIDIA `nvfortran`, NAG `nagfor`) and C/C++ (`gcc` 4.9+, Apple `clang`, MSVC `cl.exe`, Intel `icx`).
  - Added multi-platform installation guide for the Fortran Package Manager (`fpm`) across macOS (Homebrew), Windows/macOS/Linux (Conda/Mamba), Windows standalone binaries/MSYS2, and Linux distributions.
  - Clarified mixed-language build mechanics: `fpm` automatically coordinates C and Fortran compilation when building from source; pre-compiled static libraries require only the Fortran compiler and `-lm`.

### Changed
- **Multi-Line Function Reference Layout**:
  - Split multi-function Heading 3 cards onto dedicated lines using `<w:br/>` across all 32 multi-function reference cards in Chapter 7.
  - Formatted run-on function definitions in card description paragraphs (e.g. `survis`, `nohide`, `tcolor`, `bcolor`, `mean`, `grid`, `tickout`) into clear individual lines.
  - Split multi-function signature rows onto separate lines across 115 multi-signature table cells, with full regex support for Python 3 return type annotations (`-> None;`, `-> float;`).
- **Table of Contents Navigation**:
  - Completely rebuilt Table of Contents with 91 clean, fully clickable hyperlinks (`#003366` Navy Blue, single underline).
  - Eliminated Word `w:numPr` engine interference so that 100% of chapter and section numbers and titles are interactive and navigate directly to bookmarks in Word and PDF viewers.
- **Example Gallery Usability**:
  - Added `(Source code in Examples directory)` directly beneath the plot caption in all 14 Chapter 9 sections where example source exists in `Examples/` but is omitted from print.
- **Master Function Index (203 Routines)**:
  - Expanded to 203 library routines, including dedicated individual rows for `d3bars`, `WaterfallCurve`, and `WaterfallPlot`, with all 96 Community Edition routines highlighted in bold `#1E7E34` green.

---

## [7.2.0] - 2026-09-29

### Added
- **Continuous Scientific Colormaps (`Source/Colormap.c`, `Include/colormap.h`)**:
  - Implemented 7 built-in mathematically uniform 256-node standard scientific colormaps:
    - `GPC_COLORMAP_VIRIDIS` (0): Perceptually uniform viridis colormap (purple to yellow).
    - `GPC_COLORMAP_PLASMA` (1): Perceptually uniform plasma colormap (navy to orange-yellow).
    - `GPC_COLORMAP_INFERNO` (2): High-contrast inferno colormap (black to bright yellow).
    - `GPC_COLORMAP_MAGMA` (3): Smooth magma colormap (black to purple to white).
    - `GPC_COLORMAP_COOLWARM` (4): Diverging blue-cyan-white-orange-red colormap for positive/negative balance.
    - `GPC_COLORMAP_RAINBOW` (5): Classic multi-hue spectral rainbow colormap.
    - `GPC_COLORMAP_TURBO` (6): Google Turbo smooth rainbow colormap with uniform luminance.
- **Fast Scalar-to-Color Query API (`gpc_get_color`, `gpc_get_rgb`, `gpc_colormap_eval`, `gpc_rgb`)**:
  - `gpc_get_color(colormap_id, val, minv, maxv)`: $O(1)$ query evaluating continuous scalar values into packed 24-bit TrueColor RGB integers (`0x01RRGGBB`).
  - `gpc_get_rgb(colormap_id, val, minv, maxv, &r, &g, &b)`: Returns discrete R, G, B byte components ($0\dots 255$).
  - `gpc_colormap_eval(colormap_id, t)`: Normalized parameter evaluation where $0.0 \le t \le 1.0$.
  - `gpc_rgb(r, g, b)`: Packs RGB values into 24-bit TrueColor integer.
- **High-Density Multidimensional Scatter (`gpc_scatter`)**:
  - Renders 2D scatter points where each marker's color is evaluated continuously from an associated third-dimension scalar array $z$ through any colormap.
- **Hardware-Accelerated SVG Vector Shading & Pattern Fill Engine (`Source/gpc_driver_svg.c`)**:
  - **16 Native SVG Vector Pattern Fills**: Implemented scalable SVG `<pattern>` definitions (`gpc_pat_1` to `gpc_pat_16`) delivering crisp vector crosshatches, horizontal/vertical lines, 45°/135° diagonals, fine/coarse grids, dot stipples, brick, and checkerboard textures for polygon, bar, and area shading without rasterization.
  - **Seamless Linear Vector Gradient Shading**: Built-in SVG `<linearGradient>` definitions with 33 smooth color stops supporting both vertical (`gpc_cmap_v_0`..`6`) and horizontal (`gpc_cmap_h_0`..`6`) shading orientations across all 7 scientific colormaps.
  - **Multi-Path Polygon Panel Shading (`flush_svg_panel`)**: Renders arbitrary closed multi-contour polygons with SVG `evenodd` fill-rule, boundary clipping, TrueColor RGB fill, and independent border stroke styling.
  - **Continuous Scalar Shading**: Seamless $O(1)$ scalar-to-color mapping for high-density 2D/3D datasets, scatter clouds, and continuous color bars with zero pixelation at any zoom level.
- **Continuous SVG Linear Vector Gradient Color Bars (`ColorKeyGradient`)**:
  - Emits hardware-accelerated SVG `<linearGradient>` definitions with 33 smooth color stops, bounded border framing, numerical calibration ticks, and publication typography.
- **TrueType Font Typography (`Arial.ttf` & `Swiss.ttf`)**:
  - Bundled standard TrueType font files with native UTF-8 Greek, subscript, superscript, and math symbol rendering.
- **Full Multi-Language Parity Across C, Modern Fortran, Python, and Java**:
  - Modern Fortran: `gpc_get_color`, `gpc_get_rgb`, `gpc_colormap_eval`, `gpc_scatter`, `gpc_colorkey_gradient` in `graphic.f90` and `graphic_iso.f90`.
  - Python 3: `graphic.get_color`, `graphic.get_rgb`, `graphic.colormap_eval`, `fig.scatter`, `fig.colorkey_gradient` in `graphic`.
  - Java 22: `Graphic.getColor`, `Graphic.getRgb`, `Graphic.colormapEval`, `fig.scatter`, `fig.colorkeyGradient` via Project Panama FFM.
- **Continuous Color Scale Plots Showcase Example (`Turbulence` / Section 9.26)**:
  - Demonstrates high-density continuous scientific colormap shading across 7,850 measurement points correlating Temperature against Transport, continuously shaded by $\log_{10} \text{Density}$ with vertical gradient color bar and TrueType font mathematical labels.
  - Added Section 9.26 to the Reference Manual (`MANUAL.md`, `GraphiC_Modern_Manual.docx`, `GraphiC_Modern_Manual.pdf`).

---

## [7.1.1] - 2026-09-25

### Fixed
- **Multi-Level Runtime Asset & Font Discovery (`gfopen`)**:
  - Implemented multi-tier directory hierarchy traversal in `Source/baux_portable.c` (`gfopen`) searching upward up to 5 parent directory levels from both the current working directory and the executable/library binary directory.
  - Resolved `fpm` and subfolder execution crashes where executables running from build subtrees (e.g. `build/gfortran_.../example/bartest.exe`) could not locate `Messages.dat`, `Messages.tab`, `Simplex.fnt`, `Swiss.fnt`, or `Unicode.ttf`.
  - Added case-insensitivity fallback matching across Linux, macOS, and Windows to resolve case discrepancies between filesystem and font calls (`simplex.fnt` vs `Simplex.fnt`).
  - Added safe dummy font metrics and null checks in `Source/Ttfntld.c` and `Source/Ttfntplt.c` to prevent `SIGSEGV` segmentation faults in the event of missing or unreadable font files.
  - Enhanced error diagnostics in `Source/Messagew.c` (`BuildMessageq`) with informative inline text explaining font lookup paths if message table files cannot be opened.
- **Windows Command Prompt (`cmd.exe`) Makefile Support**:
  - Corrected Windows shell detection in both root `Makefile` and Community `Makefile`. Standard Windows Command Prompt sessions without MSYS2/Cygwin now explicitly invoke `SHELL := cmd.exe` and `.SHELLFLAGS := /c`, utilizing native `if not exist $(DIR) mkdir $(DIR)` to prevent erroneous `-p` folder creation.
- **Fortran Build Integration in Makefiles & CMake**:
  - Added automatic Fortran compiler detection and Fortran example compilation targets (`make fortran`, `bin/f_*`) in Community Edition `Makefile`.
  - Added `BUILD_EXAMPLES` configuration option in Community `CMakeLists.txt` providing automated compilation of all C and Fortran example executables.

---

## [7.1.0] - 2026-09-24

### Added
- **Desktop Auto-Preview (`svgopen` / `svg_open`)**:
  - Automatically launches the generated plot in the user's default desktop web browser or system SVG viewer using native operating system dispatch (`open` on macOS, `start` on Windows, `xdg-open` on Linux).
  - Passing `svgopen(NULL)` or an empty string (`""`) automatically retrieves and opens the companion HTML viewer (e.g. `myplot.html`) for the active session, featuring interactive mouse-wheel zoom, drag-to-pan, view reset, and live vector streaming updates.
  - Passing a filename (e.g. `svgopen("myplot.svg")`) opens the raw vector graphic directly in the viewer associated with `.svg` files by the host operating system.
  - Full multi-language parity across C, Fortran (`gpc_svgopen`), Python (`fig.svgopen()`), and Java (`GraphiC.svgopen()`).
- **Live Plot Initialization Auto-Preview (`GPC_AUTOPREVIEW=1`)**:
  - Setting the environment variable `GPC_AUTOPREVIEW=1` automatically invokes `svgopen(NULL)` immediately upon `bgnplot()`, launching the companion browser viewer as soon as plotting begins without interrupting stream execution.
- **Network Telemetry Push (`svgpush` / `svg_push`)**:
  - Transmits rendered SVG plots over the network to remote HTTP/HTTPS ingest endpoints or live telemetry dashboards using standard `POST` requests with `Content-Type: image/svg+xml`.
  - Modernizes and supersedes the historical socket streaming (`tkfpush`), enabling cloud compute clusters and IoT devices (such as live water temperature monitors) to stream plots directly without shell scripts or manual file copying.
- **Comprehensive SVG Parity Test Suite (`svgtest` / §9.24)**:
  - Added new multi-language test program (`Examples/Svgtest.c`, `fortran/examples/svgtest.f90`, `python/examples/svgtest.py`, `java/examples/Svgtest.java`) verifying tight bounding-box cropping, telemetry push, and desktop auto-preview.
  - Added Section 9.24 to the Visual Gallery in the reference manual.
- **Dedicated Architecture Guide: Section 2.6 "How You See GraphiC Plots"**:
  - Comprehensive guide and comparison table detailing dual-file output (`myplot.svg` vs `myplot.html`), viewer target matrix, and OS launch mechanisms.
- **Fortran Package Manager (`fpm`) Integration**:
  - Native `fpm.toml` manifest in the project root allowing GraphiC to be consumed as a direct git or path dependency in modern Fortran workflows.
- **Master Function Index Parity (191 Routines)**:
  - Section 10 expanded to 191 library routines, all cross-referenced with 100% multi-language parity across C, Python, Java 22, and Fortran 2003.

---

## [7.0.1] - 2026-09-10

### Added
- **SVG Tight Bounding-Box Auto-Cropping (`svgcrop` / `svg_crop`)**:
  - Implemented dynamic bounding-box tracking across all 2D and 3D graphics operations: vector lines, polylines, Hershey vector font strokes, TrueType Bézier glyphs, and filled polygon panels.
  - ANSI C `tmpfile()` stream buffering in `Source/gpc_driver_svg.c` to defer `<svg>` header and background `<rect id="bg">` generation until plot completion.
  - Default **0.25 inch** (1/4") margin surrounding all drawn graphical elements (axes, labels, ticks, curves, titles, bars, legends, and fills).
  - Configurable margin control API across all supported languages:
    - **C / C++**: `void svgcrop(float margin_inches);` and macro `#define svg_crop svgcrop` in `graphic.h`.
    - **Fortran 2003**: `gpc_svgcrop(margin)` and `gpc_svg_crop(margin)` in `graphic.f90` / `graphic_iso.f90`.
    - **Python 3**: `svgcrop(margin_inches=0.25)` and alias `svg_crop()` in `procedural.py`, `fig.svgcrop()` in `figure.py`.
    - **Java 22 (FFM)**: `Graphic.svgcrop(float marginInches)` and `fig.svgcrop(float marginInches)` in `Graphic.java` and `Figure.java`.
  - Full-canvas fallback: Passing a negative margin (e.g. `svgcrop(-1.0f)`) disables auto-cropping and restores the standard uncropped 8.5" × 11" landscape canvas (`viewBox="0 0 8192 6266"`).
- **Typography Architecture & Counter Documentation**:
  - Clarified typography comparison (§4.7 / §10): explicitly detailed architectural differences between open vector stroke fonts (`simplex`, `duplex`, `triplex`), multi-piece closed-loop outline fonts (`swiss`, `news`), and TrueType closed Bézier contours with `fill-rule="evenodd"`.
  - Added complete OpenXML reference specification for `svgcrop` in Section 7.3 (§7.3 Page and Axis Options) of `docs/MANUAL.md` and `docs/GraphiC_Modern_Manual.docx`.
  - Added `svgcrop` to Section 10 Master Function Index (now 189 routines) with Full Parity status across C, Fortran, Python, and Java.

### Changed
- Default SVG export behavior now automatically crops to the plot bounding box + 0.25" margin, eliminating excess letter-sized white space when embedding plots into LaTeX documents, Microsoft Word, web pages, or journal figures.
- Background `<rect id="bg">` in SVG output now matches the calculated `viewBox` coordinates and dimensions.
- Resolved method signature collision in Python `Figure.vector` vs `Figure.line_vector` to support both directional arrow vectors and raw sheet vectors.

### Verified
- Recompiled and verified all example test suites across C (35 examples), Fortran (19 examples), Python (25 examples), and Java (19 examples).
- Regenerated all 70+ test SVGs across the repository with tight `viewBox` coordinates.
- Audited all 191 production SVGs repository-wide with Python's XML parser; 100% well-formed with matching viewports.

---

## [7.0.0] - 2026-09-07 — Initial Release of GraphiC 2026

### Added
- **64-Bit Multi-Platform Engine**:
  - Full native 64-bit portability for macOS (Apple Silicon ARM64 & Intel x86_64), Linux (x86_64 & AArch64), and 64-bit Windows.
  - Zero-dependency architecture: pure standard libc and libm. No OpenGL, X11, Qt, Cairo, or FreeType runtime requirements.
  - Sub-millisecond cold-start latency (<0.002s) and minimal library footprint (~655 KB static, ~407 KB dynamic).
  - Modernized portable memory and OS abstraction layer (`Source/memory_portable.c`, `Source/baux_portable.c`) replacing legacy 16-bit DOS far pointers and segment registers.
- **Zero-Dependency Vector SVG 1.1 Driver**:
  - High-throughput vector SVG 1.1 driver (`Source/gpc_driver_svg.c`) with 256-color palettes, 16 hatch/cross-hatch fill patterns, line attributes, and Hershey vector stroke text.
  - Responsive standalone SVGs paired with lightweight zero-framework HTML5 pan/zoom inspection viewers (`viewer.html`).
- **Modern TrueType (`.ttf`) Outline Engine**:
  - Built-in zero-dependency TrueType engine (`Source/Truetype.c`, `Source/Ttfntld.c`, `Source/Ttfntplt.c`).
  - Quadratic Bézier B-spline curve tessellation with user-configurable error tolerance (`SetTTMaxError`).
  - Native UTF-8 multi-byte string decoding: transparently parses 2-byte Greek characters (α, β, γ, δ, λ, μ, π, σ, ω), mathematical operators (≤, ≥, ≠, ≈, ≡, ∞, ∇, ∂, √), and symbols (°C, ±).
  - OpenType/TrueType Format 4 `cmap` Unicode BMP resolution.
  - `evenodd` fill rule integration ensuring hollow counters/holes in letters (O, B, 8, R, e, a, %, Δ, θ).
  - Pairwise kerning evaluation via SFNT `kern` table (`SetTTKerning`).
- **First-Class Polyglot Language Bindings**:
  - **Modern Fortran 2003+**: Complete ISO_C_BINDING module interface (`fortran/src/graphic.f90`, `fortran/src/graphic_iso.f90`) with 19 runnable examples.
  - **Python 3**: Idiomatic package (`python/graphic/`) with procedural API matching classic GraphiC routines, object-oriented `Figure` context managers, NumPy array support, and inline Jupyter notebook rendering.
  - **Java 22**: Object-oriented SDK (`com.sciend.graphic`) leveraging Project Panama Foreign Function & Memory (FFM) API with zero-JNI downcalls.
- **Specialized Engineering Visualizations**:
  - Built-in routines for RF Smith charts (`smdraw`, `smgrid`), chemical ternary phase diagrams (`trigrid`, `tricnt`), 3D hidden-surface mesh removal (`surmatr`), 2D contouring with arbitrary labels (`conmatr`), polar grids (`polgrid`), and simultaneous multi-axis systems (`simuplot`).
  - 23 classic Hershey vector stroke font plates (`simplex`, `complex`, `triplex`, `gothic`, `greek`, `script`, `swiss`).
- **Complete Modern Documentation Suite**:
  - 190-page modernized reference manual (`docs/GraphiC_Modern_Manual.docx` and PDF) featuring professional typography, callout alerts, and code examples.
  - **Section 10 Master Function Index**: 188+ library routines cross-referenced with exact calling signatures, C source files (`Source/*.c`), manual section hyperlinks, and multi-language parity status.
  - Classical Hershey Font Plate Appendices: Full glyph tables for 14 classical typefaces.
  - Developer Guides: `README.md`, `QUICKSTART.md`, `RECIPES.md`, and Drupal comparison documentation.
- **Modern Build Infrastructure**:
  - Cross-platform CMake build configuration (`CMakeLists.txt`).
  - POSIX / GNU Make system (`Makefile`) with targets for C static/shared libraries, Fortran modules, Java compilation, and automated test suite execution (`make run_c_examples`, `make run_fortran_examples`).
  - Automated digital delivery packager (`package_distribution.py`) for Lemon Squeezy releases.
