"""
High-level Object-Oriented Scientific Plotting Interface for GraphiC
"""

import os
import ctypes
from ctypes import POINTER
from ._bindings import get_lib, GPC_FUNQ
from .constants import (
    COLOR_MAP, PATTERN_MAP,
    BLACK, BLUE, RED, GREEN, WHITE, DRK_GRAY, YELLOW, BROWN,
    SOLID, SYM_CIRCLE
)

def _to_c_float_array(seq):
    # Support numpy arrays, lists, tuples
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

class Figure:
    """
    Publication-Quality Scientific Graphics Figure powered by GraphiC.
    """
    def __init__(self, width=8.0, height=6.0, bgcolor="white", tkf_name="graphic_plot.tkf", background=None, output_path=None):
        self.width = float(width)
        self.height = float(height)
        if background is not None:
            bgcolor = background
        self.bgcolor = _resolve_color(bgcolor, WHITE)
        self.tkf_name = tkf_name
        self.output_path = output_path
        self.lib = get_lib()
        self._is_active = False
        self._has_axes = False
        self._svg_content = None

        self._start()

    def _start(self):
        # Initialise plot session
        self.lib.bgnplot(1, ord('g'), self.tkf_name.encode('utf-8'))
        self.lib.metricunits(0)
        self.lib.startplot(self.bgcolor)
        self.lib.page(ctypes.c_float(self.width), ctypes.c_float(self.height))
        self.lib.area2d(ctypes.c_float(self.width * 0.82), ctypes.c_float(self.height * 0.80))
        self._is_active = True

    def box(self, color="black"):
        self.lib.color(_resolve_color(color, BLACK))
        self.lib.box()

    def title(self, text):
        self.lib.heading(text.encode('utf-8'))

    def xlabel(self, text):
        self.lib.xname(text.encode('utf-8'))

    def ylabel(self, text):
        self.lib.yname(text.encode('utf-8'))

    def zlabel(self, text):
        self.lib.zname(text.encode('utf-8'))

    def setup_axes(self, xmin, xstep, xmax, ymin, ystep, ymax,
                   xformat="%g", yformat="%g", color="black"):
        self.lib.color(_resolve_color(color, BLACK))
        self.lib.graf(
            xformat.encode('utf-8'), ctypes.c_float(xmin), ctypes.c_float(xstep), ctypes.c_float(xmax),
            yformat.encode('utf-8'), ctypes.c_float(ymin), ctypes.c_float(ystep), ctypes.c_float(ymax)
        )
        self._has_axes = True

    def autoscale(self, x, y, nxdiv=5, nydiv=5, color="black"):
        c_x, n_x = _to_c_float_array(x)
        c_y, n_y = _to_c_float_array(y)
        n = min(n_x, n_y)
        self.lib.color(_resolve_color(color, BLACK))
        self.lib.scales(nxdiv, nydiv, c_x, c_y, n)
        self._has_axes = True

    def plot(self, x, y, color=None, pattern=None, symbol=0, thickness=None):
        c_x, n_x = _to_c_float_array(x)
        c_y, n_y = _to_c_float_array(y)
        n = min(n_x, n_y)

        if not self._has_axes:
            self.autoscale(x, y)

        clr_idx = _resolve_color(color, BLUE)
        pat_idx = _resolve_pattern(pattern, SOLID)

        self.lib.color(clr_idx)
        self.lib.dashf(pat_idx)
        self.lib.curve(c_x, c_y, n, int(symbol))

    def scatter(self, x, y, color=None, symbol=SYM_CIRCLE, interval=1, symbol_id=None):
        c_x, n_x = _to_c_float_array(x)
        c_y, n_y = _to_c_float_array(y)
        n = min(n_x, n_y)

        if symbol_id is not None:
            symbol = symbol_id

        if not self._has_axes:
            self.autoscale(x, y)

        clr_idx = _resolve_color(color, RED)
        self.lib.color(clr_idx)
        self.lib.sympick(int(symbol))
        self.lib.curve(c_x, c_y, n, -max(1, int(interval)))

    # Authentic GraphiC routine names
    heading = title
    xname = xlabel
    yname = ylabel
    zname = zlabel
    graf = setup_axes
    curve = plot

    def area2d(self, xlen, ylen):
        self.lib.area2d(ctypes.c_float(xlen), ctypes.c_float(ylen))

    def svgcrop(self, margin_inches=0.25):
        """Set SVG crop margin in inches (default 0.25). Pass < 0 to disable."""
        self.lib.svgcrop(ctypes.c_float(margin_inches))
        return self

    svg_crop = svgcrop

    def svgopen(self, filepath=None):
        """Open SVG plot in default browser or viewer."""
        c_p = filepath.encode('utf-8') if filepath else None
        return int(self.lib.svgopen(c_p))

    svg_open = svgopen

    def svgpush(self, url=None, filepath=None):
        """Push SVG plot over HTTP/HTTPS to endpoint."""
        c_u = url.encode('utf-8') if url else None
        c_f = filepath.encode('utf-8') if filepath else None
        return int(self.lib.svgpush(c_u, c_f))

    svg_push = svgpush

    def physor(self, x, y):
        self.lib.physor(ctypes.c_float(x), ctypes.c_float(y))

    def setup_3d_axes(self, x_range, y_range, z_range, title=None, x_label=None, y_label=None, z_label=None, view=(8.8, 3.66, 3.0)):
        self.lib.color(BLACK)
        self.lib.volm3d(ctypes.c_float(2.0), ctypes.c_float(2.0), ctypes.c_float(1.5))
        self.lib.vuabs(ctypes.c_float(view[0]), ctypes.c_float(view[1]), ctypes.c_float(view[2]))
        if title:
            self.lib.d3head(title.encode('latin-1'), ord('t'))
        if x_label:
            self.lib.x3name(x_label.encode('latin-1'))
        if y_label:
            self.lib.y3name(y_label.encode('latin-1'))
        if z_label:
            self.lib.z3name(z_label.encode('latin-1'))
        self.lib.graf3d(ctypes.c_float(x_range[0]), ctypes.c_float(x_range[1]), ctypes.c_float(x_range[2]),
                        ctypes.c_float(y_range[0]), ctypes.c_float(y_range[1]), ctypes.c_float(y_range[2]),
                        ctypes.c_float(z_range[0]), ctypes.c_float(z_range[1]), ctypes.c_float(z_range[2]))

    def surface(self, z_matrix, color="brown"):
        c_z, _ = _to_c_float_array(z_matrix)
        ny = len(z_matrix)
        nx = len(z_matrix[0]) if ny > 0 else 0
        self.lib.color(_resolve_color(color, BROWN))
        self.lib.survis(b"top")
        self.lib.surmat(c_z, nx, ny)

    def plot_surface(self, z_matrix, xdim, ydim,
                     xlim=(-3.14, 1.0, 3.14),
                     ylim=(0.0, 1.0, 3.14),
                     zlim=(-1.0, 0.5, 1.0),
                     view=(8.8, 3.66, 3.0),
                     top_color="brown", bot_color="magenta",
                     title=None):
        """
        Render a 3-D surface mesh with hidden line removal.
        """
        self.lib.color(BLACK)
        self.lib.box()
        self.lib.tickout(1)

        # 3D workbox and viewpoint
        self.lib.volm3d(ctypes.c_float(2.0), ctypes.c_float(1.0), ctypes.c_float(1.0))
        self.lib.vuabs(ctypes.c_float(view[0]), ctypes.c_float(view[1]), ctypes.c_float(view[2]))

        # Axis scaling
        self.lib.graf3d(
            ctypes.c_float(xlim[0]), ctypes.c_float(xlim[1]), ctypes.c_float(xlim[2]),
            ctypes.c_float(ylim[0]), ctypes.c_float(ylim[1]), ctypes.c_float(ylim[2]),
            ctypes.c_float(zlim[0]), ctypes.c_float(zlim[1]), ctypes.c_float(zlim[2])
        )

        self.lib.tcolor(_resolve_color(top_color))
        self.lib.bcolor(_resolve_color(bot_color))
        self.lib.d3base(1, 0)
        self.lib.survis(b"both")

        c_z, _ = _to_c_float_array(z_matrix)
        self.lib.surmat(c_z, int(xdim), int(ydim))

        if title:
            self.lib.color(_resolve_color(top_color))
            self.lib.d3head(title.encode('utf-8'), ord('t'))

    def contour(self, z_matrix, x_coords, y_coords,
                zmin, zstep, zmax, color="black", label_interval=1):
        """
        Render a 2-D contour plot.
        """
        c_z, _ = _to_c_float_array(z_matrix)
        c_x, nx = _to_c_float_array(x_coords)
        c_y, ny = _to_c_float_array(y_coords)

        self.lib.color(_resolve_color(color, BLACK))
        self.lib.conmat(
            c_z, c_x, nx, c_y, ny,
            ctypes.c_float(zmin), ctypes.c_float(zstep), ctypes.c_float(zmax),
            None, label_interval
        )

    def smith_chart(self, grid_color="black", traces=None, trace_color="blue"):
        """
        Render a radio-frequency Smith chart.
        traces: list of (r, x) impedance points or list of coordinates
        """
        self.lib.smdraw(_resolve_color(grid_color, BLACK))
        if traces:
            self.lib.color(_resolve_color(trace_color, BLUE))
            for r, x in traces:
                self.lib.plotrx(ctypes.c_float(r), ctypes.c_float(x))

    def panelinch(self, x, y, pattern=1, border=1):
        """
        Draw a filled polygonal panel in inch units.
        pattern: 1..16 (vector hatch/stipple/brick), -15..0 (solid color), 50..447 (dithers & 256 colors)
        """
        c_x, nx = _to_c_float_array(x)
        c_y, ny = _to_c_float_array(y)
        n = min(nx, ny)
        self.lib.panelinch(c_x, c_y, n, int(pattern), int(border))
        self.lib.panproc()

    def bar(self, x, y, nxdiv=5, bwid=0.8, mode=0, colors=None, patterns=None):
        """
        Draw a 2-D bar chart.
        """
        c_x, nx = _to_c_float_array(x)
        c_y, ny = _to_c_float_array(y)
        n = min(nx, ny)

        c_colors = None
        if colors:
            c_colors = (ctypes.c_int * len(colors))(*[_resolve_color(c) for c in colors])

        c_patterns = None
        if patterns:
            c_patterns = (ctypes.c_int * len(patterns))(*[int(p) for p in patterns])

        self.lib.bar(int(nxdiv), c_x, c_y, n, ctypes.c_float(bwid), int(mode), c_colors, c_patterns)

    def barchart(self, nbars, xarray, yarray, npts=None, width=0.8, carray=None, typearray=None):
        """High-level convenience alias for bar()."""
        return self.bar(xarray, yarray, nxdiv=nbars, bwid=width, mode=0, colors=carray, patterns=typearray)

    def pie(self, values, captions=None, colors=None, offsets=None, size=2.0):
        """
        Draw a publication-quality exploded pie chart.
        """
        dim = len(values)
        c_seg = (ctypes.c_float * dim)(*[float(v) for v in values])

        if colors:
            c_clr = (ctypes.c_int * dim)(*[-abs(_resolve_color(c)) if isinstance(c, str) else int(c) for c in colors])
        else:
            default_clrs = [-4, -1, -2, -5, -3, -6, -7, -8]
            c_clr = (ctypes.c_int * dim)(*[default_clrs[i % len(default_clrs)] for i in range(dim)])

        if offsets:
            c_offset = (ctypes.c_float * dim)(*[float(o) for o in offsets])
        else:
            c_offset = (ctypes.c_float * dim)(*[0.0 for _ in range(dim)])

        if captions:
            c_cap = (ctypes.c_char_p * dim)(*[c.encode('utf-8') for c in captions])
        else:
            c_cap = (ctypes.c_char_p * dim)(*[b"" for _ in range(dim)])

        self.lib.pieplot(dim, c_seg, c_clr, c_offset, c_cap, ctypes.c_float(size), 14, 0)

    def polgrid(self, rmax, rmin=0.0, mode=0, labels=None, nray=8, col=0):
        """Draw polar coordinate grid."""
        c_labels = None
        lab_num = 0
        if labels:
            lab_num = len(labels)
            c_labels = (ctypes.c_char_p * lab_num)(*[l.encode('latin-1') for l in labels])
        self.lib.polgrid(ctypes.c_float(rmax), ctypes.c_float(rmin), int(mode), c_labels, lab_num, int(nray), ctypes.c_char(col))

    def polcurve(self, r, theta, mark=0):
        """Plot curve in polar coordinates."""
        c_r, nr = _to_c_float_array(r)
        c_th, nth = _to_c_float_array(theta)
        n = min(nr, nth)
        self.lib.polcurve(c_r, c_th, n, int(mark))

    def xlog(self, xmin, xmax, form="%-1.1f", yorig=0.0, ystep=1.0, ymax=10.0):
        """Setup semi-log X axis."""
        self.lib.xlog(ctypes.c_float(xmin), ctypes.c_float(xmax), form.encode('latin-1'),
                      ctypes.c_float(yorig), ctypes.c_float(ystep), ctypes.c_float(ymax))

    def lglgscle(self, x, y):
        """Setup log-log scaling from data."""
        c_x, nx = _to_c_float_array(x)
        c_y, ny = _to_c_float_array(y)
        self.lib.lglgscle(c_x, c_y, min(nx, ny))

    def vector(self, x1, y1, x2, y2, head_angle=2.0, head_size=0.06, opt="01"):
        """Draw vector arrow in user coordinates."""
        self.lib.uvector(ctypes.c_float(x1), ctypes.c_float(y1),
                         ctypes.c_float(x2), ctypes.c_float(y2),
                         ctypes.c_float(head_angle), ctypes.c_float(head_size),
                         opt.encode('latin-1'))

    def d3bars(self, bwid=0.2):
        """Enable 3D perspective bar rendering in surface plots."""
        self.lib.d3bars(ctypes.c_float(bwid))

    def ternary_names(self, a_name, b_name, c_name):
        """Set names for the 3 sides of a ternary plot."""
        self.lib.aname(a_name.encode('latin-1'))
        self.lib.bname(b_name.encode('latin-1'))
        self.lib.cname(c_name.encode('latin-1'))

    def simuplot(self, nplots):
        """Reserve multiple simultaneous plot contexts."""
        self.lib.simuplot(int(nplots))

    def context(self, icontext):
        """Switch active coordinate context."""
        self.lib.context(int(icontext))

    def orel(self, dx, dy):
        """Relative offset for coordinate origin."""
        self.lib.orel(ctypes.c_float(dx), ctypes.c_float(dy))

    def physor(self, x, y):
        """Physical origin in inches."""
        self.lib.physor(ctypes.c_float(x), ctypes.c_float(y))

    def axesoff(self, bits):
        """Selectively disable axis drawing elements."""
        self.lib.axesoff(int(bits))

    def pltfnt(self, x, y, text, size=0.15, angle=0):
        """Plot text string in user coordinates."""
        self.lib.pltfnt(ctypes.c_float(x), ctypes.c_float(y), text.encode('latin-1'), ctypes.c_float(size), int(angle))

    def render(self):
        """
        Terminates the plot drawing and captures the generated SVG output.
        """
        if self._is_active:
            self.lib.endplot()
            self.lib.stopplot()
            self._is_active = False

        svg_path = "graphic_plot.svg"
        if hasattr(self.lib, "gpc_get_svg_path"):
            path_bytes = self.lib.gpc_get_svg_path()
            if path_bytes:
                svg_path = path_bytes.decode('utf-8')

        if os.path.exists(svg_path):
            with open(svg_path, "r", encoding="utf-8") as f:
                self._svg_content = f.read()
        return self._svg_content

    def save(self, output_path):
        """Save vector plot directly to an SVG file."""
        content = self.render()
        if content:
            with open(output_path, "w", encoding="utf-8") as f:
                f.write(content)

    def save_html(self, output_path):
        """Save interactive pan/zoom HTML5 viewer."""
        self.render()
        svg_path = "graphic_plot.svg"
        if hasattr(self.lib, "gpc_get_svg_path"):
            path_bytes = self.lib.gpc_get_svg_path()
            if path_bytes:
                svg_path = path_bytes.decode('utf-8')

        html_path = svg_path.replace(".svg", ".html")
        if os.path.exists(html_path):
            with open(html_path, "r", encoding="utf-8") as f_in:
                data = f_in.read()
            with open(output_path, "w", encoding="utf-8") as f_out:
                f_out.write(data)

    def box3d(self):
        """Draw 3D perspective bounding cuboid enclosure frame."""
        self.lib.box3d()

    def cross(self, iscross=1):
        """Select between boxed perimeter axes and center-crossed axes."""
        c_flag = b'1' if iscross else b'0'
        self.lib.cross(c_flag)

    def cross_at(self, x, y):
        """Draw coordinate axes intersecting at arbitrary coordinate (x, y)."""
        self.lib.CrossAt(ctypes.c_float(x), ctypes.c_float(y))

    def frame(self, frameon=1, ftick=1):
        """Draw rectangular bounding frame with inward or outward facing tick marks."""
        self.lib.frame(bytes([frameon]), bytes([ftick]))

    def tickht(self, height):
        """Set length of coordinate axis tick marks in inches."""
        self.lib.tickht(ctypes.c_float(height))

    def tickout(self, is_out=1):
        """Set whether axis tick marks point outward or inward."""
        self.lib.tickout(int(is_out))

    def numht(self, height):
        """Set character height for axis numeric tick labels in inches."""
        self.lib.numht(ctypes.c_float(height))

    def titlht(self, height):
        """Set character height for main plot title heading in inches."""
        self.lib.titlht(ctypes.c_float(height))

    def axnamht(self, height):
        """Set character height for coordinate axis titles in inches."""
        self.lib.axnamht(ctypes.c_float(height))

    def upright(self, uplab=1):
        """Force numeric tick labels on vertical Y-axis to be drawn horizontally."""
        self.lib.upright(bytes([uplab]))

    def acrop(self, flag=1):
        """Configure automatic plot boundary clipping on or off."""
        self.lib.acrop(bytes([flag]))

    def rotate(self, isrot=1):
        """Rotate page orientation by 90 degrees."""
        self.lib.rotate(bytes([isrot]))

    def pgshift(self, x, y):
        """Translate physical plotting origin across page surface in inches."""
        self.lib.pgshift(ctypes.c_float(x), ctypes.c_float(y))

    def fgrid(self, fon=1, ndiv=5):
        """Draw full rectangular Cartesian grid mesh with specified divisions."""
        self.lib.fgrid(int(fon), int(ndiv))

    def xfgrid(self, fon=1, nx=5):
        """Draw vertical grid lines with custom sub-interval divisions."""
        self.lib.xfgrid(int(fon), int(nx))

    def yfgrid(self, fon=1, ny=5):
        """Draw horizontal grid lines with custom sub-interval divisions."""
        self.lib.yfgrid(int(fon), int(ny))

    def d3line(self, x1, y1, z1, x2, y2, z2):
        """Draw 3D perspective line between two coordinates."""
        self.lib.d3line(ctypes.c_float(x1), ctypes.c_float(y1), ctypes.c_float(z1),
                        ctypes.c_float(x2), ctypes.c_float(y2), ctypes.c_float(z2))

    def curv3d(self, x, y, z):
        """Plot 3D spatial trajectory curve in perspective coordinate space."""
        cx, nx = _to_c_float_array(x)
        cy, ny = _to_c_float_array(y)
        cz, nz = _to_c_float_array(z)
        n = min(nx, ny, nz)
        self.lib.curv3d(cx, cy, cz, n)

    def d3color(self, colors, nlevels=None, mesh=0):
        """Configure color elevation mapping palette for 3D surface meshes."""
        if nlevels is None:
            nlevels = len(colors)
        c_colors = (ctypes.c_int * len(colors))(*colors)
        self.lib.d3color(c_colors, int(nlevels), bytes([mesh]))

    def sclfix(self, isfixed=1):
        """Lock isometric 1:1:1 aspect ratio scaling across all 3D axes."""
        self.lib.sclfix(int(isfixed))

    def nohide(self, visible=1):
        """Disable hidden-line removal for transparent wireframe mesh rendering."""
        self.lib.nohide(int(visible))

    def vuangl(self, phi, theta, radius=10.0):
        """Set 3D perspective viewpoint elevation and azimuth angles in degrees."""
        self.lib.vuangl(ctypes.c_float(phi), ctypes.c_float(theta), ctypes.c_float(radius))

    def contcolor(self, colors, border=0, style=None):
        """Set color mapping scheme for contour elevation levels."""
        c_colors = (ctypes.c_int * len(colors))(*colors)
        if style is not None:
            c_style = (ctypes.c_int * len(style))(*style)
            self.lib.contcolor_fixed(c_colors, int(border), c_style)
        else:
            self.lib.contcolor_fixed(c_colors, int(border), None)

    def curvfill(self, x, y, x1=None, y1=None, color="black", border=0):
        """Fill area beneath or between curves with solid color or pattern."""
        cx, nx = _to_c_float_array(x)
        cy, ny = _to_c_float_array(y)
        if x1 is not None and y1 is not None:
            cx1, nx1 = _to_c_float_array(x1)
            cy1, ny1 = _to_c_float_array(y1)
        else:
            cx1 = (ctypes.c_float * 0)()
            cy1 = (ctypes.c_float * 0)()
            nx1 = 0
        c_idx = _resolve_color(color, BLACK)
        self.lib.curvfill(cx, cy, nx, cx1, cy1, nx1, c_idx, int(border))

    def errorbar(self, x, xmin, xmax, y, ymin, ymax):
        """Draw vertical or horizontal error bars on data points."""
        self.lib.errorbar(ctypes.c_float(x), ctypes.c_float(xmin), ctypes.c_float(xmax),
                          ctypes.c_float(y), ctypes.c_float(ymin), ctypes.c_float(ymax))

    def scatplot(self, dot=1):
        """Generate 2D scatter plot with specified markers and no connecting lines."""
        self.lib.scatplot(bytes([dot]))

    def histogram(self, x, y, type=0, colors=None):
        """Construct statistical frequency distribution bar histogram."""
        cx, nx = _to_c_float_array(x)
        cy, ny = _to_c_float_array(y)
        n = min(nx, ny)
        c_cols = None
        if colors is not None:
            c_cols = (ctypes.c_int * len(colors))(*colors)
        self.lib.histogram(cx, cy, n, int(type), c_cols)

    def ctline(self, text, height=0.15):
        """Render centered line of text using active vector font."""
        self.lib.ctline(text.encode('utf-8'), ctypes.c_float(height))

    def ltline(self, text, height=0.15):
        """Render left-justified line of text using active vector font."""
        self.lib.ltline(text.encode('utf-8'), ctypes.c_float(height))

    def rtline(self, text, height=0.15):
        """Render right-justified line of text using active vector font."""
        self.lib.rtline(text.encode('utf-8'), ctypes.c_float(height))

    def strwidth(self, text, height=0.15):
        """Compute physical width of a text string in inches."""
        return float(self.lib.strwidth(text.encode('utf-8'), ctypes.c_float(height)))

    def subsym(self, ch):
        """Set vertical baseline downward shift fraction for subscript typesetting."""
        c_ch = ord(ch) if isinstance(ch, str) else ch
        self.lib.subsym(bytes([c_ch]))

    def legend(self, marker, text, style=0, height=0.12, color="black"):
        """Add an entry to plot legend box."""
        c_col = _resolve_color(color, BLACK)
        self.lib.legend(int(marker), text.encode('utf-8'), int(style), ctypes.c_float(height), c_col)

    def legpos(self, entries, x, y, border=1):
        """Set physical location, entry count, and border style for plot legend box."""
        self.lib.legpos(int(entries), ctypes.c_float(x), ctypes.c_float(y), int(border))

    def loglog(self, xorig, xmax, yorig, ymax):
        """Draw full log-log coordinate axes with logarithmic grid."""
        self.lib.loglog(ctypes.c_float(xorig), ctypes.c_float(xmax),
                        ctypes.c_float(yorig), ctypes.c_float(ymax))

    def wedge(self, rmin, rmax, thmin, thmax, pattern=0, border=1):
        """Draw solid or patterned angular sector wedge on polar plot."""
        self.lib.wedge(ctypes.c_float(rmin), ctypes.c_float(rmax),
                       ctypes.c_float(thmin), ctypes.c_float(thmax),
                       int(pattern), int(border))

    def stack(self, stacked=1):
        """Draw stacked 3D contour elevation slices."""
        self.lib.stack(bytes([stacked]))

    def plotrho(self, rho, theta):
        """Plot reflection coefficient curve on microwave Smith chart."""
        self.lib.plotrho(ctypes.c_float(rho), ctypes.c_float(theta))

    def smithcir(self, u, v, radius):
        """Draw constant resistance or reactance circle on Smith chart."""
        self.lib.smithcir(ctypes.c_float(u), ctypes.c_float(v), ctypes.c_float(radius))

    def smove(self, dell):
        """Move pen to coordinate in Smith chart coordinate space."""
        self.lib.smove(ctypes.c_float(dell))

    def tline(self, width=1):
        """Draw straight line segment in physical inches."""
        self.lib.tline(int(width))

    def tcurve(self, width=1):
        """Plot polyline curve using absolute physical inch coordinates."""
        self.lib.tcurve(int(width))

    def plotadd(self):
        """Enable multi-pass vector layering onto existing plot frame without clear."""
        self.lib.plotadd()

    def plotabsolute(self, flag=1):
        """Move pen cursor to absolute device pixel coordinate."""
        self.lib.plotabsolute(int(flag))

    def new_file(self, name):
        """Redirect subsequent vector output stream to a new file destination."""
        self.lib.NewFile(name.encode('utf-8'))

    def dateit(self, font='a'):
        """Append automated timestamp and version watermark to bottom margin."""
        c_f = ord(font) if isinstance(font, str) else font
        self.lib.dateit(bytes([c_f]))

    def linesp(self, ratio):
        """Set line spacing ratio for multi-line Hershey text blocks."""
        self.lib.linesp(ctypes.c_float(ratio))

    def lmargin(self, marginch):
        """Set left margin in inches for Hershey text layout."""
        self.lib.lmargin(ctypes.c_float(marginch))

    def rmargin(self, marginch):
        """Set right margin in inches for Hershey text layout."""
        self.lib.rmargin(ctypes.c_float(marginch))

    def tmargin(self, marginch):
        """Set top margin in inches for Hershey text layout."""
        self.lib.tmargin(ctypes.c_float(marginch))

    def charspc(self, width):
        """Set character spacing / kerning offset in device pixels."""
        self.lib.charspc(int(width))

    def skew(self, slant):
        """Set slant angle / italicization skew for Hershey vector glyphs."""
        self.lib.skew(ctypes.c_float(slant))

    def widen(self, fact):
        """Set character aspect ratio width scaling factor."""
        self.lib.widen(ctypes.c_float(fact))

    def tfont(self, wid):
        """Set pen stroke thickness for vector fonts in device pixels."""
        self.lib.tfont(int(wid))

    def tifont(self, wid):
        """Set pen stroke thickness for vector fonts in physical inches."""
        self.lib.tifont(ctypes.c_float(wid))

    def fntchg(self, sym):
        """Change active font family or Hershey stroke font code."""
        c_sym = ord(sym) if isinstance(sym, str) else sym
        self.lib.fntchg(bytes([c_sym]))

    def supsym(self, ch):
        """Set vertical baseline upward shift fraction for superscript typesetting."""
        c_ch = ord(ch) if isinstance(ch, str) else ch
        self.lib.supsym(bytes([c_ch]))

    def putlabel(self, text, xpos, ypos, height=0.15, border=0, units=0):
        """Render multi-line text block with optional border in user or inch units."""
        self.lib.putlabel(text.encode('utf-8'), ctypes.c_float(xpos), ctypes.c_float(ypos),
                          ctypes.c_float(height), int(border), int(units))

    def set_name_space(self, axis: int, factor: float):
        """Set label spacing factor for specified axis (1=X, 2=Y, 3=Z)."""
        self.lib.SetNameSpace(int(axis), ctypes.c_float(factor))

    def color(self, c):
        """Set current active drawing color."""
        self.lib.color(_resolve_color(c))

    def ylog(self, xorig, xstep, xmax, yorig, ymax, format="%g"):
        """Draw semi-logarithmic axes with linear X-axis and logarithmic Y-axis."""
        self.lib.ylog(format.encode('utf-8'), ctypes.c_float(xorig), ctypes.c_float(xstep),
                      ctypes.c_float(xmax), ctypes.c_float(yorig), ctypes.c_float(ymax))

    def prtfnt(self, xinch, yinch, text, height, angle=0):
        """Render text in page inch coordinates."""
        self.lib.prtfnt(ctypes.c_float(xinch), ctypes.c_float(yinch), text.encode('utf-8'),
                        ctypes.c_float(height), int(angle))

    def fillfont(self, fill: int = 1):
        """Toggle filled font glyph rendering (1 = fill, 0 = outline)."""
        self.lib.fillfont(int(fill))

    def fontfill(self, fill: int, pattern: int = 0, border: int = 0):
        """Configure interior fill pattern and border color for font glyphs."""
        self.lib.fontfill(int(fill), int(pattern), int(border))

    def d3base(self, isbase: int = 1, colorb: int = 0):
        """Configure baseline elevation plane and color for 3D surface meshes."""
        self.lib.d3base(int(isbase), int(colorb))

    def _repr_svg_(self):
        """IPython / Jupyter Notebook inline SVG rendering hook."""
        return self.render()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.render()
        if self.output_path:
            self.save(self.output_path)

    # --- Full Parity Additions for Figure ---
    def box_plot(self, x, y, outlier=1, symnum=1, width=0.5):
        """Draw a box-and-whisker plot at x for array y."""
        c_y, n = _to_c_float_array(y)
        self.lib.BoxPlot(ctypes.c_float(x), c_y, n, int(outlier), int(symnum), ctypes.c_float(width))

    def waterfall_plot(self, x, y, z):
        """Render 3D waterfall / spectral cascade plot."""
        c_x, nx = _to_c_float_array(x)
        c_y, ny = _to_c_float_array(y)
        row_ptrs = (ctypes.POINTER(ctypes.c_float) * ny)()
        rows = []
        for i in range(ny):
            c_row, _ = _to_c_float_array(z[i])
            rows.append(c_row)
            row_ptrs[i] = c_row
        self.lib.WaterfallPlot(c_x, c_y, row_ptrs, nx, ny)

    WaterfallPlot = waterfall_plot

    def waterfall_curve(self, x, y, z):
        """Draw a single 2D spectral curve in 3D perspective space with hidden line removal."""
        c_x, npts = _to_c_float_array(x)
        c_z, _ = _to_c_float_array(z)
        self.lib.WaterfallCurve(c_x, ctypes.c_float(y), c_z, npts)

    WaterfallCurve = waterfall_curve

    def plane3d(self, wplane='Z', pos=0.0, hide=1):
        """Draw a cutting plane in 3D perspective space."""
        c_p = ord(wplane) if isinstance(wplane, str) else wplane
        c_h = ord(hide) if isinstance(hide, str) else hide
        self.lib.plane3d(bytes([c_p]), ctypes.c_float(pos), bytes([c_h]))

    def colorkey_l(self, minv, maxv, intensity, legllx, leglly, legurx, legury, labint=1, format="%-3.2f", fontc='1', height=0.15, angle=0):
        """Draw linear color key / legend bar."""
        c_intens = (ctypes.c_int * len(intensity))(*intensity)
        c_font = ord(fontc) if isinstance(fontc, str) else fontc
        self.lib.ColorKeyL(ctypes.c_float(minv), ctypes.c_float(maxv), c_intens, len(intensity),
                          ctypes.c_float(legllx), ctypes.c_float(leglly), ctypes.c_float(legurx), ctypes.c_float(legury),
                          int(labint), format.encode('utf-8'), bytes([c_font]), ctypes.c_float(height), int(angle))

    def colorkey_n(self, values, intensity, legllx, leglly, legurx, legury, labint=1, format="%-3.2f", fontc='1', height=0.15, angle=0):
        """Draw non-linear color key / legend bar."""
        c_vals, _ = _to_c_float_array(values)
        c_intens = (ctypes.c_int * len(intensity))(*intensity)
        c_font = ord(fontc) if isinstance(fontc, str) else fontc
        self.lib.ColorKeyN(c_vals, c_intens, len(intensity),
                          ctypes.c_float(legllx), ctypes.c_float(leglly), ctypes.c_float(legurx), ctypes.c_float(legury),
                          int(labint), format.encode('utf-8'), bytes([c_font]), ctypes.c_float(height), int(angle))

    def colorkey_gradient(self, minv, maxv, colormap=0, legllx=7.0, leglly=1.0, legurx=7.4, legury=6.0,
                          nticks=8, format="%-3.1f", fontc=chr(200), height=0.12, angle=0):
        """Plot continuous gradient color key / color bar with ticks and labels."""
        c_fmt = format.encode('utf-8') if isinstance(format, str) else format
        c_font = ord(fontc) if isinstance(fontc, str) else int(fontc)
        self.lib.ColorKeyGradient(ctypes.c_float(minv), ctypes.c_float(maxv), int(colormap),
                                  ctypes.c_float(legllx), ctypes.c_float(leglly),
                                  ctypes.c_float(legurx), ctypes.c_float(legury),
                                  int(nticks), c_fmt, ctypes.c_char(c_font),
                                  ctypes.c_float(height), int(angle))

    def erfcurve(self, x, y, sym=0):
        """Plot an error function / probability curve."""
        c_x, nx = _to_c_float_array(x)
        c_y, ny = _to_c_float_array(y)
        self.lib.erfcurve(c_x, c_y, min(nx, ny), int(sym))

    def erfgrid(self, ymin, ystep, ymax, yformat="%g"):
        """Draw probability / normal distribution grid lines."""
        self.lib.erfgrid(ctypes.c_float(ymin), ctypes.c_float(ystep), ctypes.c_float(ymax), yformat.encode('utf-8'))

    def erfscales(self, nydiv, y):
        """Calibrate probability / error function scales."""
        c_y, n = _to_c_float_array(y)
        self.lib.erfscales(int(nydiv), c_y, n)

    def ellipse(self, xc, yc, xr, yr, rotation=0.0, th1=0.0, th2=360.0, degrees=1, units=0):
        """Draw an ellipse or elliptical arc."""
        self.lib.ellipse(ctypes.c_float(xc), ctypes.c_float(yc), ctypes.c_float(xr), ctypes.c_float(yr),
                        ctypes.c_float(rotation), ctypes.c_float(th1), ctypes.c_float(th2),
                        bytes([1 if degrees else 0]), int(units))

    def fillellipse(self, onoff=1, pattern=0, border=0):
        """Toggle ellipse interior filling."""
        self.lib.fillellipse(int(onoff), int(pattern), int(border))

    def spline(self, x, y, n1, s1=0.0, s2=0.0):
        """Compute cubic spline interpolation."""
        c_x, n = _to_c_float_array(x)
        c_y, _ = _to_c_float_array(y)
        c_xn = (ctypes.c_float * n1)()
        c_yn = (ctypes.c_float * n1)()
        self.lib.spline(c_x, c_y, n, int(n1), ctypes.c_float(s1), ctypes.c_float(s2), c_xn, c_yn)
        return list(c_xn), list(c_yn)

    def blank(self, attr=0):
        """Clear or blank current plot canvas region."""
        self.lib.blank(int(attr))

    def wedge(self, rmin, rmax, thmin, thmax, pattern=0, border=0):
        """Draw an annular polar wedge."""
        self.lib.wedge(ctypes.c_float(rmin), ctypes.c_float(rmax), ctypes.c_float(thmin), ctypes.c_float(thmax),
                      int(pattern), int(border))

    def uvector(self, xfrom, yfrom, xto, yto, ltow=1.0, size=0.1, type="A"):
        """Draw directional vector in user coordinates."""
        self.lib.uvector(ctypes.c_float(xfrom), ctypes.c_float(yfrom), ctypes.c_float(xto), ctypes.c_float(yto),
                        ctypes.c_float(ltow), ctypes.c_float(size), type.encode('utf-8'))

    def line_vector(self, x1, y1, x2, y2):
        """Draw 2D vector in sheet coordinates."""
        self.lib.vector(ctypes.c_float(x1), ctypes.c_float(y1), ctypes.c_float(x2), ctypes.c_float(y2))

    def d3grid(self, subdiv=5, label=0):
        """Draw 3D perspective grid lines."""
        self.lib.d3grid(int(subdiv), int(label))

    def d3pltfnt(self, x, y, z, text, height=0.2, angle=0):
        """Plot text in 3D perspective coordinates."""
        self.lib.d3pltfnt(ctypes.c_float(x), ctypes.c_float(y), ctypes.c_float(z),
                         text.encode('utf-8'), ctypes.c_float(height), int(angle))

    def xfgrid(self, fon=1, nx=5):
        """Draw fine vertical X grid subdivisions."""
        self.lib.xfgrid(int(fon), int(nx))

    def yfgrid(self, fon=1, ny=5):
        """Draw fine horizontal Y grid subdivisions."""
        self.lib.yfgrid(int(fon), int(ny))

    def trimat(self, m1, m2, z, dim1, zmin, zstep, zmax, lnstyle, border=0):
        """Plot ternary contour matrix diagram."""
        c_m1, dim = _to_c_float_array(m1)
        c_m2, _ = _to_c_float_array(m2)
        c_z, _ = _to_c_float_array(z)
        c_lnstyle, _ = _to_c_int_array(lnstyle)
        self.lib.trimat(c_m1, c_m2, c_z, int(dim), int(dim1), ctypes.c_float(zmin),
                        ctypes.c_float(zstep), ctypes.c_float(zmax), c_lnstyle, int(border))

    def trifun(self, fun, dim, zmin, zstep, zmax, lnstyle, border=0):
        """Plot ternary contour function diagram."""
        c_lnstyle, _ = _to_c_int_array(lnstyle)
        c_fun = GPC_FUNQ(fun) if callable(fun) and not isinstance(fun, ctypes._CFuncPtr) else fun
        self.lib.trifun(c_fun, int(dim), ctypes.c_float(zmin),
                        ctypes.c_float(zstep), ctypes.c_float(zmax), c_lnstyle, int(border))

    def surmat(self, zmat, nx=None, ny=None):
        """Plot 3D perspective surface from 1D contiguous or 2D array."""
        if isinstance(zmat, (list, tuple)) and zmat and isinstance(zmat[0], (list, tuple)):
            actual_ny = len(zmat)
            actual_nx = len(zmat[0])
            nx = nx if nx is not None else actual_nx
            ny = ny if ny is not None else actual_ny
        c_z, _ = _to_c_float_array(zmat)
        self.lib.surmat(c_z, int(nx), int(ny))

    def surmat2(self, zmat, nx=None, ny=None):
        """Plot 3D perspective surface from 2D array or row-pointer matrix."""
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
        self.lib.surmat2(c_rows, int(nx), int(ny))

    def surfun(self, fun, ixpts, xdel, iypts, ydel):
        """Plot 3D perspective surface from analytical function z = f(x, y)."""
        c_fun = GPC_FUNQ(fun) if callable(fun) and not isinstance(fun, ctypes._CFuncPtr) else fun
        self.lib.surfun(c_fun, int(ixpts), ctypes.c_float(xdel), int(iypts), ctypes.c_float(ydel))

    def usertoinch(self, x, y):
        """Convert user data coordinates to page inch coordinates."""
        xi = ctypes.c_float()
        yi = ctypes.c_float()
        self.lib.usertoinch(ctypes.c_float(x), ctypes.c_float(y), ctypes.byref(xi), ctypes.byref(yi))
        return xi.value, yi.value

    def usertopix(self, x, y, z=0.0):
        """Convert user coordinates to pixel coordinates."""
        xp = ctypes.c_int()
        yp = ctypes.c_int()
        self.lib.usertopix(ctypes.c_float(x), ctypes.c_float(y), ctypes.c_float(z), ctypes.byref(xp), ctypes.byref(yp))
        return xp.value, yp.value

    def std_dev(self, x):
        """Compute sample standard deviation of float array."""
        c_x, n = _to_c_float_array(x)
        return self.lib.StdDev(c_x, n)

    stddev = std_dev
    StdDev = std_dev

    def settolerance(self, tolerance=0.001):
        """Configure 3D hidden-line collinearity removal tolerance."""
        self.lib.settolerance(ctypes.c_float(tolerance))


