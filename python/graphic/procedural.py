"""
Authentic GraphiC procedural state machine API for Python 3.
Provides direct 1:1 parity with the classic C/Fortran GraphiC routine names:
bgnplot -> page -> startplot -> area2d/physor -> box -> heading/xname/yname -> graf -> curve -> endplot/stopplot
"""

import os
import sys
import ctypes
from ctypes import c_int, c_float, c_char_p, POINTER

from ._bindings import get_lib, GPC_FUNQ
from .constants import (
    COLOR_MAP, PATTERN_MAP,
    BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, WHITE,
    DRK_GRAY, LGT_BLUE, LGT_GREEN, LGT_CYAN, LGT_RED, LGT_MAGENTA,
    YELLOW, BRT_WHITE,
    SOLID, DOTTED, DASHED, LONG_DASH, CHAIN_DASH,
    SYM_SQUARE, SYM_CIRCLE, SYM_DEL, SYM_PLUS, SYM_CROSS,
    SYM_DIAMOND, SYM_TRIANGLE, SYM_STICK_FIGURE, SYM_FEET_PLUS,
    SYM_HLINE, SYM_VLINE, SYM_BOX, SYM_DOT, SYM_BIG_DOT, SYM_BIGGER_DOT
)

def _resolve_color(c, default=BLACK):
    if c is None:
        return default
    if isinstance(c, int):
        return c
    if isinstance(c, str):
        return COLOR_MAP.get(c.lower().strip(), default)
    return default

def _resolve_pattern(p, default=SOLID):
    if p is None:
        return default
    if isinstance(p, int):
        return p
    if isinstance(p, str):
        return PATTERN_MAP.get(p.lower().strip(), default)
    return default

def _to_c_float_array(seq):
    try:
        import numpy as np
        if isinstance(seq, np.ndarray):
            seq = seq.astype(np.float32).flatten()
            return (ctypes.c_float * len(seq))(*seq), len(seq)
    except ImportError:
        pass

    if isinstance(seq, (list, tuple)) and seq and isinstance(seq[0], (list, tuple)):
        flat = [float(x) for row in seq for x in row]
    else:
        flat = [float(x) for x in seq]
    return (ctypes.c_float * len(flat))(*flat), len(flat)

def _to_c_int_array(seq):
    try:
        import numpy as np
        if isinstance(seq, np.ndarray):
            seq = seq.astype(np.int32).flatten()
            return (ctypes.c_int * len(seq))(*seq), len(seq)
    except ImportError:
        pass

    if isinstance(seq, (list, tuple)) and seq and isinstance(seq[0], (list, tuple)):
        flat = [int(x) for row in seq for x in row]
    else:
        flat = [int(x) for x in seq]
    return (ctypes.c_int * len(flat))(*flat), len(flat)

# --- Procedural State Machine Functions ---

def bgnplot(mode=0, driver='S', filename="graphic_plot.svg"):
    """Initialize a GraphiC plotting session."""
    lib = get_lib()
    drv_code = ord(driver) if isinstance(driver, str) and len(driver) == 1 else int(driver)
    if isinstance(filename, str):
        fn_bytes = filename.encode('utf-8')
    else:
        fn_bytes = bytes(filename)
    lib.bgnplot(int(mode), drv_code, fn_bytes)

def page(width=8.0, height=6.0):
    """Set the physical page dimensions (inches or mm)."""
    lib = get_lib()
    lib.page(c_float(width), c_float(height))

def startplot(bgcolor=BRT_WHITE):
    """Start a new plot canvas with the specified background color."""
    lib = get_lib()
    clr = _resolve_color(bgcolor, BRT_WHITE)
    lib.startplot(int(clr))

def area2d(xlen, ylen):
    """Define the 2D plot window dimensions within the page."""
    lib = get_lib()
    lib.area2d(c_float(xlen), c_float(ylen))

def svgcrop(margin_inches: float = 0.25):
    """
    Set SVG plot auto-cropping margin in inches.
    margin_inches >= 0 enables tight cropping with specified margin (default 0.25").
    margin_inches < 0 disables cropping (full letter sheet).
    """
    lib = get_lib()
    lib.svgcrop(c_float(margin_inches))

svg_crop = svgcrop

def svgopen(filepath: str | None = None) -> int:
    """
    Open SVG plot in default browser or system viewer.
    If filepath is None, opens the most recently rendered plot.
    """
    lib = get_lib()
    c_p = filepath.encode('utf-8') if filepath else None
    return int(lib.svgopen(c_p))

svg_open = svgopen

def svgpush(url: str | None = None, filepath: str | None = None) -> int:
    """
    Push SVG plot over HTTP/HTTPS (POST) to a web endpoint or dashboard.
    If url is None, reads GPC_SVG_PUSH_URL environment variable.
    If filepath is None, pushes the most recently rendered plot.
    """
    lib = get_lib()
    c_u = url.encode('utf-8') if url else None
    c_f = filepath.encode('utf-8') if filepath else None
    return int(lib.svgpush(c_u, c_f))

svg_push = svgpush

def physor(x, y):
    """Set the physical origin (bottom-left) of the plot in page units."""
    lib = get_lib()
    lib.physor(c_float(x), c_float(y))

def orel(dx, dy):
    """Offset the origin relative to the current physical position."""
    lib = get_lib()
    lib.orel(c_float(dx), c_float(dy))

def metricunits(enable=0):
    """Set coordinate scaling units (0 = inches, 1 = millimeters)."""
    lib = get_lib()
    lib.metricunits(int(enable))

def box():
    """Draw a bounding box around the active 2D/3D plot area."""
    lib = get_lib()
    lib.box()

def pbox():
    """Draw a border around the entire physical page boundary."""
    lib = get_lib()
    lib.pbox()

def numht(height):
    """Set axis tick numeral height in physical units."""
    lib = get_lib()
    lib.numht(ctypes.c_float(height))

def xnamht(height):
    """Set X-axis label height in physical units."""
    lib = get_lib()
    lib.xnamht(ctypes.c_float(height))

def ynamht(height):
    """Set Y-axis label height in physical units."""
    lib = get_lib()
    lib.ynamht(ctypes.c_float(height))

def heading(title):
    """Draw the primary plot title above the active plot area."""
    lib = get_lib()
    lib.heading(title.encode('utf-8') if isinstance(title, str) else bytes(title))

def xname(label):
    """Set the horizontal X-axis label."""
    lib = get_lib()
    lib.xname(label.encode('utf-8') if isinstance(label, str) else bytes(label))

def yname(label):
    """Set the vertical Y-axis label."""
    lib = get_lib()
    lib.yname(label.encode('utf-8') if isinstance(label, str) else bytes(label))

def zname(label):
    """Set the 3D elevation Z-axis label."""
    lib = get_lib()
    lib.z3name(label.encode('utf-8') if isinstance(label, str) else bytes(label))

def set_name_space(axis, factor):
    """Set spacing between axis tick numbers and axis label."""
    lib = get_lib()
    lib.SetNameSpace(int(axis), ctypes.c_float(factor))

def hardcopy(mode='n'):
    """Set hardcopy execution mode ('n' for non-blocking)."""
    lib = get_lib()
    b = mode.encode('ascii') if isinstance(mode, str) else bytes([mode])
    lib.hardcopy(b)

def color(clr):
    """Set current drawing foreground color."""
    lib = get_lib()
    c = _resolve_color(clr, BLACK)
    lib.color(int(c))

def tcolor(clr):
    """Set text color."""
    lib = get_lib()
    c = _resolve_color(clr, BLACK)
    lib.tcolor(int(c))

def bcolor(clr):
    """Set background/border color."""
    lib = get_lib()
    c = _resolve_color(clr, BLACK)
    lib.bcolor(int(c))

def dashf(pattern):
    """Set the vector line dashing pattern (SOLID, DASHED, DOTTED, etc.)."""
    lib = get_lib()
    p = _resolve_pattern(pattern, SOLID)
    lib.dashf(int(p))

def sympick(symbol):
    """Select the active point marker symbol."""
    lib = get_lib()
    lib.sympick(int(symbol))

def symbol(x, y, sym_id):
    """Draw an individual marker symbol at coordinates (x, y)."""
    lib = get_lib()
    lib.symbol(c_float(x), c_float(y), int(sym_id))

def graf(xfmt, xmin, xstep, xmax, yfmt, ymin, ystep, ymax):
    """Calibrate linear 2D Cartesian axes with numeric labels and tick marks."""
    lib = get_lib()
    lib.graf(
        xfmt.encode('utf-8') if isinstance(xfmt, str) else bytes(xfmt),
        c_float(xmin), c_float(xstep), c_float(xmax),
        yfmt.encode('utf-8') if isinstance(yfmt, str) else bytes(yfmt),
        c_float(ymin), c_float(ystep), c_float(ymax)
    )

def scales(nxdiv, nydiv, x, y, n=None):
    """Autoscale axes based on coordinate arrays."""
    lib = get_lib()
    c_x, n_x = _to_c_float_array(x)
    c_y, n_y = _to_c_float_array(y)
    total_n = min(n_x, n_y) if n is None else int(n)
    lib.scales(int(nxdiv), int(nydiv), c_x, c_y, total_n)

def curve(x, y, n=None, symbol=0):
    """Draw a calibrated 2D curve or symbol scatter series."""
    lib = get_lib()
    c_x, n_x = _to_c_float_array(x)
    c_y, n_y = _to_c_float_array(y)
    total_n = min(n_x, n_y) if n is None else int(n)
    lib.curve(c_x, c_y, total_n, int(symbol))

def endplot():
    """Conclude the current plot session, flushing buffers and outputting vector files."""
    lib = get_lib()
    return lib.endplot()

def stopplot():
    """Terminate the GraphiC plotting system, closing all drivers."""
    lib = get_lib()
    lib.stopplot()

def fillfont(fill=1):
    """Toggle filled font glyph rendering (1 = fill, 0 = outline)."""
    lib = get_lib()
    lib.fillfont(int(fill))

def fontfill(fill=1, pattern=0, border=0):
    """Configure interior fill pattern and border color for font glyphs."""
    lib = get_lib()
    lib.fontfill(int(fill), int(pattern), int(border))

def d3base(isbase=1, colorb=0):
    """Configure baseline elevation plane and color for 3D surface meshes."""
    lib = get_lib()
    lib.d3base(int(isbase), int(colorb))

def d3color(colors, nlevels=None, mesh=1):
    """Configure 3D surface color palette and optional mesh drawing."""
    lib = get_lib()
    if nlevels is None:
        nlevels = len(colors)
    c_colors = (ctypes.c_int * len(colors))(*[int(c) for c in colors])
    mesh_b = bytes([mesh]) if isinstance(mesh, int) else (mesh.encode('ascii') if isinstance(mesh, str) else mesh)
    lib.d3color(c_colors, int(nlevels), mesh_b)

def plotabsolute(flag=1):
    """Enable or disable absolute inch plotting coordinates."""
    lib = get_lib()
    lib.plotabsolute(int(flag))

# --- Parity Additions for GraphiC Procedural & Helper Suite ---

def box_plot(x, y, outlier=1, symnum=1, width=0.5):
    """Draw a box-and-whisker plot at x for array y."""
    lib = get_lib()
    c_y, n = _to_c_float_array(y)
    lib.BoxPlot(ctypes.c_float(x), c_y, n, int(outlier), int(symnum), ctypes.c_float(width))

def waterfall_plot(x, y, z):
    """Render 3D waterfall / spectral cascade plot."""
    lib = get_lib()
    c_x, nx = _to_c_float_array(x)
    c_y, ny = _to_c_float_array(y)
    # 2D z array
    row_ptrs = (POINTER(ctypes.c_float) * ny)()
    rows = []
    for i in range(ny):
        c_row, _ = _to_c_float_array(z[i])
        rows.append(c_row)
        row_ptrs[i] = c_row
    lib.WaterfallPlot(c_x, c_y, row_ptrs, nx, ny)

WaterfallPlot = waterfall_plot

def waterfall_curve(x, y, z):
    """Draw a single 2D spectral curve in 3D perspective space with hidden line removal."""
    lib = get_lib()
    c_x, npts = _to_c_float_array(x)
    c_z, _ = _to_c_float_array(z)
    lib.WaterfallCurve(c_x, ctypes.c_float(y), c_z, npts)

WaterfallCurve = waterfall_curve

def plane3d(wplane='Z', pos=0.0, hide=1):
    """Draw a cutting plane in 3D perspective space."""
    lib = get_lib()
    c_p = ord(wplane) if isinstance(wplane, str) else wplane
    c_h = ord(hide) if isinstance(hide, str) else hide
    lib.plane3d(bytes([c_p]), ctypes.c_float(pos), bytes([c_h]))

def colorkey_l(minv, maxv, intensity, legllx, leglly, legurx, legury, labint=1, format="%-3.2f", fontc='1', height=0.15, angle=0):
    """Draw linear color key / legend bar."""
    lib = get_lib()
    c_intens = (ctypes.c_int * len(intensity))(*intensity)
    c_font = ord(fontc) if isinstance(fontc, str) else fontc
    lib.ColorKeyL(ctypes.c_float(minv), ctypes.c_float(maxv), c_intens, len(intensity),
                  ctypes.c_float(legllx), ctypes.c_float(leglly), ctypes.c_float(legurx), ctypes.c_float(legury),
                  int(labint), format.encode('utf-8'), bytes([c_font]), ctypes.c_float(height), int(angle))

def colorkey_n(values, intensity, legllx, leglly, legurx, legury, labint=1, format="%-3.2f", fontc='1', height=0.15, angle=0):
    """Draw non-linear color key / legend bar."""
    lib = get_lib()
    c_vals, _ = _to_c_float_array(values)
    c_intens = (ctypes.c_int * len(intensity))(*intensity)
    c_font = ord(fontc) if isinstance(fontc, str) else fontc
    lib.ColorKeyN(c_vals, c_intens, len(intensity),
                  ctypes.c_float(legllx), ctypes.c_float(leglly), ctypes.c_float(legurx), ctypes.c_float(legury),
                  int(labint), format.encode('utf-8'), bytes([c_font]), ctypes.c_float(height), int(angle))

def erfcurve(x, y, sym=0):
    """Plot an error function / probability curve."""
    lib = get_lib()
    c_x, nx = _to_c_float_array(x)
    c_y, ny = _to_c_float_array(y)
    lib.erfcurve(c_x, c_y, min(nx, ny), int(sym))

def erfgrid(ymin, ystep, ymax, yformat="%g"):
    """Draw probability / normal distribution grid lines."""
    lib = get_lib()
    lib.erfgrid(ctypes.c_float(ymin), ctypes.c_float(ystep), ctypes.c_float(ymax), yformat.encode('utf-8'))

def erfscales(nydiv, y):
    """Calibrate probability / error function scales."""
    lib = get_lib()
    c_y, n = _to_c_float_array(y)
    lib.erfscales(int(nydiv), c_y, n)

def usertoinch(x, y):
    """Convert user data coordinates to page inch coordinates."""
    lib = get_lib()
    xi = ctypes.c_float()
    yi = ctypes.c_float()
    lib.usertoinch(ctypes.c_float(x), ctypes.c_float(y), ctypes.byref(xi), ctypes.byref(yi))
    return xi.value, yi.value

def usertopix(x, y, z=0.0):
    """Convert user coordinates to pixel coordinates."""
    lib = get_lib()
    xp = ctypes.c_int()
    yp = ctypes.c_int()
    lib.usertopix(ctypes.c_float(x), ctypes.c_float(y), ctypes.c_float(z), ctypes.byref(xp), ctypes.byref(yp))
    return xp.value, yp.value

def intopix(inches):
    """Convert inch coordinates to pixels."""
    lib = get_lib()
    return lib.intopix(ctypes.c_float(inches))

def pixtoin(pixels):
    """Convert pixels to inches."""
    lib = get_lib()
    return lib.pixtoin(int(pixels))

def ellipse(xc, yc, xr, yr, rotation=0.0, th1=0.0, th2=360.0, degrees=1, units=0):
    """Draw an ellipse or elliptical arc."""
    lib = get_lib()
    lib.ellipse(ctypes.c_float(xc), ctypes.c_float(yc), ctypes.c_float(xr), ctypes.c_float(yr),
                ctypes.c_float(rotation), ctypes.c_float(th1), ctypes.c_float(th2),
                bytes([1 if degrees else 0]), int(units))

def fillellipse(onoff=1, pattern=0, border=0):
    """Toggle ellipse interior filling."""
    lib = get_lib()
    lib.fillellipse(int(onoff), int(pattern), int(border))

def spline(x, y, n1, s1=0.0, s2=0.0):
    """Compute cubic spline interpolation."""
    lib = get_lib()
    c_x, n = _to_c_float_array(x)
    c_y, _ = _to_c_float_array(y)
    c_xn = (ctypes.c_float * n1)()
    c_yn = (ctypes.c_float * n1)()
    lib.spline(c_x, c_y, n, int(n1), ctypes.c_float(s1), ctypes.c_float(s2), c_xn, c_yn)
    return list(c_xn), list(c_yn)

def blank(attr=0):
    """Clear or blank current plot canvas region."""
    lib = get_lib()
    lib.blank(int(attr))

def wedge(rmin, rmax, thmin, thmax, pattern=0, border=0):
    """Draw an annular polar wedge."""
    lib = get_lib()
    lib.wedge(ctypes.c_float(rmin), ctypes.c_float(rmax), ctypes.c_float(thmin), ctypes.c_float(thmax),
              int(pattern), int(border))

def uvector(xfrom, yfrom, xto, yto, ltow=1.0, size=0.1, type="A"):
    """Draw directional vector in user coordinates."""
    lib = get_lib()
    lib.uvector(ctypes.c_float(xfrom), ctypes.c_float(yfrom), ctypes.c_float(xto), ctypes.c_float(yto),
                ctypes.c_float(ltow), ctypes.c_float(size), type.encode('utf-8'))

def vector(x1, y1, x2, y2, ltow=1.0, size=0.1, type="A"):
    """Draw directional vector in physical page inches."""
    lib = get_lib()
    b_type = type.encode('utf-8') if isinstance(type, str) else bytes(type)
    lib.vector(ctypes.c_float(x1), ctypes.c_float(y1), ctypes.c_float(x2), ctypes.c_float(y2),
               ctypes.c_float(ltow), ctypes.c_float(size), b_type)

def uline(xfrom, yfrom, xto, yto, newcurv=1):
    """Draw a straight line segment between two points in user/data coordinates."""
    lib = get_lib()
    lib.uline(ctypes.c_float(xfrom), ctypes.c_float(yfrom),
              ctypes.c_float(xto), ctypes.c_float(yto), ctypes.c_int(newcurv))

def linch(x1, y1, x2, y2, newcurv=1):
    """Draw a straight line segment between two points in physical page inches."""
    lib = get_lib()
    lib.linch(ctypes.c_float(x1), ctypes.c_float(y1),
              ctypes.c_float(x2), ctypes.c_float(y2), ctypes.c_char(bytes([int(newcurv)])))

def line(xfrom, yfrom, xto, yto, newcurv=1):
    """Draw a straight line segment between two points in device raster pixels."""
    lib = get_lib()
    lib.line(ctypes.c_int(xfrom), ctypes.c_int(yfrom),
             ctypes.c_int(xto), ctypes.c_int(yto), ctypes.c_int(newcurv))

def d3grid(subdiv=5, label=0):
    """Draw 3D perspective grid lines."""
    lib = get_lib()
    lib.d3grid(int(subdiv), int(label))

def prtfnt(xinch, yinch, text, height=0.2, angle=0):
    """Plot formatted text in physical inches using active font."""
    lib = get_lib()
    b_text = text.encode('utf-8') if isinstance(text, str) else bytes(text)
    lib.prtfnt(ctypes.c_float(xinch), ctypes.c_float(yinch), b_text, ctypes.c_float(height), int(angle))

def pltfnt(x, y, text, height=0.2, angle=0):
    """Plot formatted text in user coordinates using active font."""
    lib = get_lib()
    b_text = text.encode('utf-8') if isinstance(text, str) else bytes(text)
    lib.pltfnt(ctypes.c_float(x), ctypes.c_float(y), b_text, ctypes.c_float(height), int(angle))

def d3pltfnt(x, y, z, text, height=0.2, angle=0):
    """Plot text in 3D perspective coordinates."""
    lib = get_lib()
    lib.d3pltfnt(ctypes.c_float(x), ctypes.c_float(y), ctypes.c_float(z),
                 text.encode('utf-8'), ctypes.c_float(height), int(angle))

def hls_to_rgb(h, l, s):
    """Convert HLS color values (0-255) to RGB."""
    lib = get_lib()
    r = ctypes.c_int()
    g = ctypes.c_int()
    b = ctypes.c_int()
    lib.HLStoRGB(int(h), int(l), int(s), ctypes.byref(r), ctypes.byref(g), ctypes.byref(b))
    return r.value, g.value, b.value

def rgb_to_hls(r, g, b):
    """Convert RGB color values (0-255) to HLS."""
    lib = get_lib()
    h = ctypes.c_int()
    l = ctypes.c_int()
    s = ctypes.c_int()
    lib.RGBtoHLS(int(r), int(g), int(b), ctypes.byref(h), ctypes.byref(l), ctypes.byref(s))
    return h.value, l.value, s.value

def rgb_to_cmy(r, g, b):
    """Convert RGB (0-255) to CMY."""
    lib = get_lib()
    c = ctypes.c_int()
    m = ctypes.c_int()
    y = ctypes.c_int()
    lib.RGBtoCMY(int(r), int(g), int(b), ctypes.byref(c), ctypes.byref(m), ctypes.byref(y))
    return c.value, m.value, y.value

def mean(x):
    """Compute sample mean of float array."""
    lib = get_lib()
    c_x, n = _to_c_float_array(x)
    return lib.mean(c_x, n)

def median(x):
    """Compute sample median of float array."""
    lib = get_lib()
    c_x, n = _to_c_float_array(x)
    return lib.median(c_x, n)

def minmax(xy):
    """Find minimum and maximum of data array."""
    lib = get_lib()
    c_xy, n = _to_c_float_array(xy)
    minq = ctypes.c_float()
    maxq = ctypes.c_float()
    lib.minmax(ctypes.byref(minq), ctypes.byref(maxq), c_xy, n)
    return minq.value, maxq.value

def std_dev(x):
    """Compute sample standard deviation of float array."""
    lib = get_lib()
    c_x, n = _to_c_float_array(x)
    return lib.StdDev(c_x, n)

stddev = std_dev
StdDev = std_dev

def set_tt_full(onoff=1, string=0):
    """Configure TrueType full character set rendering."""
    lib = get_lib()
    lib.SetTTFull(bytes([1 if onoff else 0]), bytes([1 if string else 0]))

def set_tt_kerning(onoff=1, string=0):
    """Configure TrueType inter-character kerning."""
    lib = get_lib()
    lib.SetTTKerning(bytes([1 if onoff else 0]), bytes([1 if string else 0]))

def set_tt_max_error(err=0.01, string=0):
    """Configure TrueType rasterization error tolerance."""
    lib = get_lib()
    lib.SetTTMaxError(ctypes.c_float(err), bytes([1 if string else 0]))

def set_tt_use_memory(onoff=1):
    """Configure TrueType RAM memory font caching."""
    lib = get_lib()
    lib.SetTTUseMemory(bytes([1 if onoff else 0]))

def settolerance(tolerance=0.001):
    """Configure 3D hidden-line collinearity removal tolerance."""
    lib = get_lib()
    lib.settolerance(ctypes.c_float(tolerance))

def saveplotq():
    """Serialize and save plot state."""
    lib = get_lib()
    lib.saveplotq()

def xfgrid(fon=1, nx=5):
    """Draw fine vertical X grid subdivisions."""
    lib = get_lib()
    lib.xfgrid(int(fon), int(nx))

def yfgrid(fon=1, ny=5):
    """Draw fine horizontal Y grid subdivisions."""
    lib = get_lib()
    lib.yfgrid(int(fon), int(ny))
def pixtouser(x_in, y_in):
    """Convert raster pixels to user data coordinates."""
    lib = get_lib()
    xo = ctypes.c_float()
    yo = ctypes.c_float()
    lib.pixtouser(int(x_in), int(y_in), ctypes.byref(xo), ctypes.byref(yo))
    return xo.value, yo.value

def rfit(x, y, nits=10, sfact=1.0, nout=100):
    """Perform robust non-linear curve fitting."""
    lib = get_lib()
    c_x, n = _to_c_float_array(x)
    c_y, _ = _to_c_float_array(y)
    xout = (ctypes.c_float * nout)()
    yout = (ctypes.c_float * nout)()
    lib.rfit(c_x, c_y, n, int(nits), ctypes.c_float(sfact), int(nout), xout, yout)
    return list(xout), list(yout)

def gpc_beep(freq=440, duration=100):
    """Audible beep alert tone."""
    lib = get_lib()
    lib.gpcBeep(int(freq), int(duration))

def set_time(timer_id=0):
    """Reset benchmark timer."""
    lib = get_lib()
    lib.set_time(int(timer_id))

def get_time(timer_id=0):
    """Read elapsed benchmark time string."""
    lib = get_lib()
    res = lib.get_time(int(timer_id))
    return res.decode('utf-8') if res else ""

def d3bars(bwid=0.2):
    """Configure 3D surface column bar width."""
    lib = get_lib()
    lib.d3bars(ctypes.c_float(bwid))

def trimat(m1, m2, z, dim1, zmin, zstep, zmax, lnstyle, border=0):
    """Plot ternary contour matrix diagram."""
    lib = get_lib()
    c_m1, dim = _to_c_float_array(m1)
    c_m2, _ = _to_c_float_array(m2)
    c_z, _ = _to_c_float_array(z)
    c_lnstyle, _ = _to_c_int_array(lnstyle)
    lib.trimat(c_m1, c_m2, c_z, int(dim), int(dim1), ctypes.c_float(zmin),
               ctypes.c_float(zstep), ctypes.c_float(zmax), c_lnstyle, int(border))

def trifun(fun, dim, zmin, zstep, zmax, lnstyle, border=0):
    """Plot ternary contour function diagram."""
    lib = get_lib()
    c_lnstyle, _ = _to_c_int_array(lnstyle)
    c_fun = GPC_FUNQ(fun) if callable(fun) and not isinstance(fun, ctypes._CFuncPtr) else fun
    lib.trifun(c_fun, int(dim), ctypes.c_float(zmin),
               ctypes.c_float(zstep), ctypes.c_float(zmax), c_lnstyle, int(border))

def surmat(zmat, nx=None, ny=None):
    """Plot 3D perspective surface from 1D contiguous or 2D array."""
    lib = get_lib()
    if isinstance(zmat, (list, tuple)) and zmat and isinstance(zmat[0], (list, tuple)):
        actual_ny = len(zmat)
        actual_nx = len(zmat[0])
        nx = nx if nx is not None else actual_nx
        ny = ny if ny is not None else actual_ny
    c_z, _ = _to_c_float_array(zmat)
    lib.surmat(c_z, int(nx), int(ny))

def surmat2(zmat, nx=None, ny=None):
    """Plot 3D perspective surface from 2D array or row-pointer matrix."""
    lib = get_lib()
    rows = list(zmat)
    actual_ny = len(rows)
    actual_nx = len(rows[0]) if actual_ny > 0 else 0
    nx = nx if nx is not None else actual_nx
    ny = ny if ny is not None else actual_ny

    c_rows = (POINTER(ctypes.c_float) * ny)()
    row_storage = []
    for i in range(ny):
        row_arr = (ctypes.c_float * nx)(*[float(v) for v in rows[i][:nx]])
        row_storage.append(row_arr)
        c_rows[i] = ctypes.cast(row_arr, POINTER(ctypes.c_float))
    lib.surmat2(c_rows, int(nx), int(ny))

def surfun(fun, ixpts, xdel, iypts, ydel):
    """Plot 3D perspective surface from analytical function z = f(x, y)."""
    lib = get_lib()
    c_fun = GPC_FUNQ(fun) if callable(fun) and not isinstance(fun, ctypes._CFuncPtr) else fun
    lib.surfun(c_fun, int(ixpts), ctypes.c_float(xdel), int(iypts), ctypes.c_float(ydel))

def rgb(r: int, g: int, b: int) -> int:
    """Pack R, G, B [0..255] into GraphiC 24-bit RGB color integer."""
    lib = get_lib()
    return lib.gpc_rgb(int(r), int(g), int(b))

def colormap_eval(colormap_id: int, t: float) -> int:
    """Evaluate colormap at normalized position t in [0.0, 1.0] -> returns 24-bit RGB color int."""
    lib = get_lib()
    return lib.gpc_colormap_eval(int(colormap_id), ctypes.c_float(t))

def get_color(colormap_id: int, value: float, minv: float, maxv: float) -> int:
    """Query colormap at a specific scalar value between minv and maxv -> returns 24-bit RGB color int."""
    lib = get_lib()
    return lib.gpc_get_color(int(colormap_id), ctypes.c_float(value), ctypes.c_float(minv), ctypes.c_float(maxv))

def get_rgb(colormap_id: int, value: float, minv: float, maxv: float):
    """Query RGB components (0..255) from colormap at a specific scalar value."""
    lib = get_lib()
    r = ctypes.c_int(0)
    g = ctypes.c_int(0)
    b = ctypes.c_int(0)
    lib.gpc_get_rgb(int(colormap_id), ctypes.c_float(value), ctypes.c_float(minv), ctypes.c_float(maxv),
                    ctypes.byref(r), ctypes.byref(g), ctypes.byref(b))
    return (r.value, g.value, b.value)

def scatter(x, y, z, minv=None, maxv=None, symnum=12, symheight=0.04, colormap=0):
    """Plot continuous colored scatter points from x, y, z data."""
    lib = get_lib()
    c_x, nx = _to_c_float_array(x)
    c_y, ny = _to_c_float_array(y)
    c_z, nz = _to_c_float_array(z)
    dim = min(nx, ny, nz)
    if minv is None:
        minv = min(z[:dim])
    if maxv is None:
        maxv = max(z[:dim])
    lib.gpc_scatter(c_x, c_y, c_z, int(dim), ctypes.c_float(minv), ctypes.c_float(maxv),
                    int(symnum), ctypes.c_float(symheight), int(colormap))

def colorkey_gradient(minv, maxv, colormap=0, legllx=7.0, leglly=1.0, legurx=7.4, legury=6.0,
                      nticks=8, format="%-3.1f", fontc=chr(200), height=0.12, angle=0):
    """Plot continuous gradient color key / color bar with ticks and labels."""
    lib = get_lib()
    c_fmt = format.encode('utf-8') if isinstance(format, str) else format
    c_font = ord(fontc) if isinstance(fontc, str) else int(fontc)
    lib.ColorKeyGradient(ctypes.c_float(minv), ctypes.c_float(maxv), int(colormap),
                         ctypes.c_float(legllx), ctypes.c_float(leglly),
                         ctypes.c_float(legurx), ctypes.c_float(legury),
                         int(nticks), c_fmt, ctypes.c_char(c_font),
                         ctypes.c_float(height), int(angle))

def font(fontname: str, esc: int | str = 200):
    """Load a font by name (supports .fnt Hershey and .ttf TrueType fonts)."""
    lib = get_lib()
    c_name = fontname.encode('utf-8') if isinstance(fontname, str) else fontname
    c_esc = ord(esc) if isinstance(esc, str) else int(esc)
    lib.gpc_load_font(c_name, ctypes.c_char(c_esc))

def skew(slant: float):
    """Set slant angle / italicization skew for vector glyphs."""
    lib = get_lib()
    lib.skew(ctypes.c_float(slant))

def widen(fact: float):
    """Scale character width aspect ratio (>1.0 expands, <1.0 condenses)."""
    lib = get_lib()
    lib.widen(ctypes.c_float(fact))

def supsym(ch: str | int | bytes):
    """Set superscript mode trigger character (e.g. '^' or '[')."""
    lib = get_lib()
    c_ch = ord(ch) if isinstance(ch, str) else (ch[0] if isinstance(ch, (bytes, bytearray)) else int(ch))
    lib.supsym(bytes([c_ch]))

def subsym(ch: str | int | bytes):
    """Set subscript mode trigger character (e.g. '_' or ']')."""
    lib = get_lib()
    c_ch = ord(ch) if isinstance(ch, str) else (ch[0] if isinstance(ch, (bytes, bytearray)) else int(ch))
    lib.subsym(bytes([c_ch]))

def tfont(wid: int):
    """Set font outline stroke thickness in device pixels."""
    lib = get_lib()
    lib.tfont(int(wid))

def tifont(wid: float):
    """Set font outline stroke thickness in physical inches."""
    lib = get_lib()
    lib.tifont(ctypes.c_float(wid))



