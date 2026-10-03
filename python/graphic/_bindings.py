"""
ctypes bindings to the native GraphiC dynamic library.
"""

import os
import sys
import ctypes
from ctypes import c_int, c_uint, c_float, c_double, c_char, c_char_p, c_void_p, POINTER

_lib = None
GPC_FUNQ = ctypes.CFUNCTYPE(c_float, c_float, c_float)

def _find_library():
    # 1. Explicit env var
    env_path = os.environ.get("GRAPHIC_LIB_PATH")
    if env_path and os.path.exists(env_path):
        return env_path

    # Determine filename by platform
    if sys.platform == "darwin":
        lib_names = ["libgraphic.dylib", "libgraphic.so"]
    elif sys.platform == "win32":
        lib_names = ["graphic.dll", "libgraphic.dll"]
    else:
        lib_names = ["libgraphic.so", "libgraphic.dylib"]

    # Search candidates
    search_dirs = [
        os.path.dirname(__file__),
        os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "lib")),
        os.path.abspath(os.path.join(os.path.dirname(__file__), "..")),
        os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "lib")),
        os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "build")),
        os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..")),
        "/usr/local/lib",
        "/opt/homebrew/lib",
    ]

    for d in search_dirs:
        for name in lib_names:
            candidate = os.path.join(d, name)
            if os.path.isfile(candidate):
                return candidate

    return None

def get_lib():
    global _lib
    if _lib is not None:
        return _lib

    lib_path = _find_library()
    if not lib_path:
        raise RuntimeError(
            "Could not locate GraphiC shared library (libgraphic.dylib / graphic.dll / libgraphic.so). "
            "Please build it using 'make' or 'cmake', or set GRAPHIC_LIB_PATH."
        )

    # Ensure headless execution doesn't block on terminal input
    os.environ.setdefault("GPC_NONINTERACTIVE", "1")

    if sys.platform == "win32" and hasattr(os, "add_dll_directory"):
        try:
            os.add_dll_directory(os.path.dirname(lib_path))
        except Exception:
            pass
        for p in os.environ.get("PATH", "").split(os.pathsep):
            if p and os.path.isdir(p):
                try:
                    os.add_dll_directory(p)
                except Exception:
                    pass

    _lib = ctypes.CDLL(lib_path)
    _setup_prototypes(_lib)
    return _lib

def _setup_prototypes(lib):
    # Life cycle
    lib.bgnplot.argtypes = [c_int, c_int, c_char_p]
    lib.bgnplot.restype = None

    lib.startplot.argtypes = [c_int]
    lib.startplot.restype = None

    lib.endplot.argtypes = []
    lib.endplot.restype = c_int

    lib.stopplot.argtypes = []
    lib.stopplot.restype = None

    # Geometry & Layout
    lib.page.argtypes = [c_float, c_float]
    lib.page.restype = None

    lib.area2d.argtypes = [c_float, c_float]
    lib.area2d.restype = None

    lib.svgcrop.argtypes = [c_float]
    lib.svgcrop.restype = None

    lib.svgopen.argtypes = [c_char_p]
    lib.svgopen.restype = c_int

    lib.svgpush.argtypes = [c_char_p, c_char_p]
    lib.svgpush.restype = c_int

    lib.metricunits.argtypes = [c_int]
    lib.metricunits.restype = None

    lib.box.argtypes = []
    lib.box.restype = None

    lib.pbox.argtypes = []
    lib.pbox.restype = None

    lib.color.argtypes = [c_int]
    lib.color.restype = None

    lib.tcolor.argtypes = [c_int]
    lib.tcolor.restype = None

    lib.bcolor.argtypes = [c_int]
    lib.bcolor.restype = None

    lib.heading.argtypes = [c_char_p]
    lib.heading.restype = None

    lib.xname.argtypes = [c_char_p]
    lib.xname.restype = None

    lib.yname.argtypes = [c_char_p]
    lib.yname.restype = None

    lib.x3name.argtypes = [c_char_p]
    lib.x3name.restype = None

    lib.y3name.argtypes = [c_char_p]
    lib.y3name.restype = None

    lib.z3name.argtypes = [c_char_p]
    lib.z3name.restype = None

    lib.d3head.argtypes = [c_char_p, c_int]
    lib.d3head.restype = None

    lib.tickout.argtypes = [c_int]
    lib.tickout.restype = None

    lib.dashf.argtypes = [c_int]
    lib.dashf.restype = None

    # 2D Scaling & Plotting
    lib.graf.argtypes = [
        c_char_p, c_float, c_float, c_float,
        c_char_p, c_float, c_float, c_float
    ]
    lib.graf.restype = None

    lib.scales.argtypes = [c_int, c_int, POINTER(c_float), POINTER(c_float), c_int]
    lib.scales.restype = None

    lib.curve.argtypes = [POINTER(c_float), POINTER(c_float), c_int, c_int]
    lib.curve.restype = None

    lib.sympick.argtypes = [c_int]
    lib.sympick.restype = None

    lib.symbol.argtypes = [c_float, c_float, c_int]
    lib.symbol.restype = None

    # 3D Plotting
    lib.volm3d.argtypes = [c_float, c_float, c_float]
    lib.volm3d.restype = None

    lib.vuabs.argtypes = [c_float, c_float, c_float]
    lib.vuabs.restype = None

    lib.graf3d.argtypes = [
        c_float, c_float, c_float,
        c_float, c_float, c_float,
        c_float, c_float, c_float
    ]
    lib.graf3d.restype = None

    lib.survis.argtypes = [c_char_p]
    lib.survis.restype = None

    lib.d3base.argtypes = [c_int, c_int]
    lib.d3base.restype = None

    lib.surmat.argtypes = [POINTER(c_float), c_int, c_int]
    lib.surmat.restype = None

    lib.curv3d.argtypes = [POINTER(c_float), POINTER(c_float), POINTER(c_float), c_int]
    lib.curv3d.restype = None

    # Contours
    lib.conmat.argtypes = [
        POINTER(c_float), POINTER(c_float), c_int, POINTER(c_float), c_int,
        c_float, c_float, c_float, POINTER(c_int), c_int
    ]
    lib.conmat.restype = None

    # Smith Chart
    lib.smdraw.argtypes = [c_int]
    lib.smdraw.restype = None

    lib.plotrx.argtypes = [c_float, c_float]
    lib.plotrx.restype = None

    lib.smove.argtypes = [c_float]
    lib.smove.restype = None

    # Panels & Patterns
    lib.panelinch.argtypes = [POINTER(c_float), POINTER(c_float), c_int, c_int, c_int]
    lib.panelinch.restype = None

    lib.panproc.argtypes = []
    lib.panproc.restype = None

    # Bar & Pie charts
    lib.bar.argtypes = [c_int, POINTER(c_float), POINTER(c_float), c_int, c_float, c_int, POINTER(c_int), POINTER(c_int)]
    lib.bar.restype = None

    lib.pieplot.argtypes = [c_int, POINTER(c_float), POINTER(c_int), POINTER(c_float), POINTER(c_char_p), c_float, c_int, c_int]
    lib.pieplot.restype = None

    # Polar plots
    lib.polgrid.argtypes = [c_float, c_float, c_int, POINTER(c_char_p), c_int, c_int, c_char]
    lib.polgrid.restype = None

    lib.polcurve.argtypes = [POINTER(c_float), POINTER(c_float), c_int, c_int]
    lib.polcurve.restype = None

    # Log plots
    lib.xlog.argtypes = [c_float, c_float, c_char_p, c_float, c_float, c_float]
    lib.xlog.restype = None

    lib.ylog.argtypes = [c_char_p, c_float, c_float, c_float, c_float, c_float]
    lib.ylog.restype = None

    lib.prtfnt.argtypes = [c_float, c_float, c_char_p, c_float, c_int]
    lib.prtfnt.restype = None

    lib.lglgscle.argtypes = [POINTER(c_float), POINTER(c_float), c_int]
    lib.lglgscle.restype = None

    # Vector fields
    lib.uvector.argtypes = [c_float, c_float, c_float, c_float, c_float, c_float, c_char_p]
    lib.uvector.restype = None

    # 3D Bars
    lib.d3bars.argtypes = [c_float]
    lib.d3bars.restype = None

    # Ternary
    lib.aname.argtypes = [c_char_p]
    lib.aname.restype = None
    lib.bname.argtypes = [c_char_p]
    lib.bname.restype = None
    lib.cname.argtypes = [c_char_p]
    lib.cname.restype = None

    # Multi-context & Layout
    lib.simuplot.argtypes = [c_int]
    lib.simuplot.restype = None
    lib.context.argtypes = [c_int]
    lib.context.restype = None
    lib.orel.argtypes = [c_float, c_float]
    lib.orel.restype = None
    lib.physor.argtypes = [c_float, c_float]
    lib.physor.restype = None
    lib.axesoff.argtypes = [c_int]
    lib.axesoff.restype = None
    lib.fillfont.argtypes = [c_int]
    lib.fillfont.restype = None
    lib.fontfill.argtypes = [c_int, c_int, c_int]
    lib.fontfill.restype = None
    lib.pltfnt.argtypes = [c_float, c_float, c_char_p, c_float, c_int]
    lib.pltfnt.restype = None

    # Driver query
    lib.gpc_get_svg_path.argtypes = []
    lib.gpc_get_svg_path.restype = c_char_p

    # New Extended Bindings for Parity
    lib.box3d.argtypes = []
    lib.box3d.restype = None

    lib.cross.argtypes = [c_char]
    lib.cross.restype = None

    lib.CrossAt.argtypes = [c_float, c_float]
    lib.CrossAt.restype = None

    lib.frame.argtypes = [c_char, c_char]
    lib.frame.restype = None

    lib.tickht.argtypes = [c_float]
    lib.tickht.restype = None

    lib.tickout.argtypes = [c_int]
    lib.tickout.restype = None

    lib.numht.argtypes = [c_float]
    lib.numht.restype = None

    lib.xnamht.argtypes = [c_float]
    lib.xnamht.restype = None

    lib.ynamht.argtypes = [c_float]
    lib.ynamht.restype = None

    lib.titlht.argtypes = [c_float]
    lib.titlht.restype = None

    lib.axnamht.argtypes = [c_float]
    lib.axnamht.restype = None

    lib.upright.argtypes = [c_char]
    lib.upright.restype = None

    lib.acrop.argtypes = [c_char]
    lib.acrop.restype = None

    lib.rotate.argtypes = [c_char]
    lib.rotate.restype = None

    lib.pgshift.argtypes = [c_float, c_float]
    lib.pgshift.restype = None

    lib.fgrid.argtypes = [c_int, c_int]
    lib.fgrid.restype = None

    lib.xfgrid.argtypes = [c_int, c_int]
    lib.xfgrid.restype = None

    lib.yfgrid.argtypes = [c_int, c_int]
    lib.yfgrid.restype = None

    lib.d3line.argtypes = [c_float, c_float, c_float, c_float, c_float, c_float]
    lib.d3line.restype = None

    lib.curv3d.argtypes = [POINTER(c_float), POINTER(c_float), POINTER(c_float), c_int]
    lib.curv3d.restype = None

    lib.d3color.argtypes = [POINTER(c_int), c_int, c_char]
    lib.d3color.restype = None

    lib.sclfix.argtypes = [c_int]
    lib.sclfix.restype = None

    lib.nohide.argtypes = [c_int]
    lib.nohide.restype = None

    lib.vuangl.argtypes = [c_float, c_float, c_float]
    lib.vuangl.restype = None

    lib.contcolor_fixed.argtypes = [POINTER(c_int), c_int, POINTER(c_int)]
    lib.contcolor_fixed.restype = None

    lib.curvfill.argtypes = [POINTER(c_float), POINTER(c_float), c_int, POINTER(c_float), POINTER(c_float), c_int, c_int, c_int]
    lib.curvfill.restype = None

    lib.errorbar.argtypes = [c_float, c_float, c_float, c_float, c_float, c_float]
    lib.errorbar.restype = None

    lib.scatplot.argtypes = [c_char]
    lib.scatplot.restype = None

    lib.histogram.argtypes = [POINTER(c_float), POINTER(c_float), c_int, c_int, POINTER(c_int)]
    lib.histogram.restype = None

    lib.ctline.argtypes = [c_char_p, c_float]
    lib.ctline.restype = None

    lib.ltline.argtypes = [c_char_p, c_float]
    lib.ltline.restype = None

    lib.rtline.argtypes = [c_char_p, c_float]
    lib.rtline.restype = None

    lib.strwidth.argtypes = [c_char_p, c_float]
    lib.strwidth.restype = c_float

    lib.subsym.argtypes = [c_char]
    lib.subsym.restype = None

    lib.legend.argtypes = [c_int, c_char_p, c_int, c_float, c_int]
    lib.legend.restype = None

    lib.legpos.argtypes = [c_int, c_float, c_float, c_int]
    lib.legpos.restype = None

    lib.loglog.argtypes = [c_float, c_float, c_float, c_float]
    lib.loglog.restype = None

    lib.wedge.argtypes = [c_float, c_float, c_float, c_float, c_int, c_int]
    lib.wedge.restype = None

    lib.stack.argtypes = [c_char]
    lib.stack.restype = None

    lib.plotrho.argtypes = [c_float, c_float]
    lib.plotrho.restype = None

    lib.smithcir.argtypes = [c_float, c_float, c_float]
    lib.smithcir.restype = None

    lib.tline.argtypes = [c_int]
    lib.tline.restype = None

    lib.tcurve.argtypes = [c_int]
    lib.tcurve.restype = None

    lib.plotadd.argtypes = []
    lib.plotadd.restype = None

    lib.plotabsolute.argtypes = [c_int]
    lib.plotabsolute.restype = None

    lib.NewFile.argtypes = [c_char_p]
    lib.NewFile.restype = None

    lib.dateit.argtypes = [c_char]
    lib.dateit.restype = None

    # Hershey font state registers
    lib.linesp.argtypes = [c_float]
    lib.linesp.restype = None

    lib.lmargin.argtypes = [c_float]
    lib.lmargin.restype = None

    lib.rmargin.argtypes = [c_float]
    lib.rmargin.restype = None

    lib.tmargin.argtypes = [c_float]
    lib.tmargin.restype = None

    lib.charspc.argtypes = [c_int]
    lib.charspc.restype = None

    lib.skew.argtypes = [c_float]
    lib.skew.restype = None

    lib.widen.argtypes = [c_float]
    lib.widen.restype = None

    lib.tfont.argtypes = [c_int]
    lib.tfont.restype = None

    lib.tifont.argtypes = [c_float]
    lib.tifont.restype = None

    lib.fntchg.argtypes = [c_char]
    lib.fntchg.restype = None

    lib.supsym.argtypes = [c_char]
    lib.supsym.restype = None

    lib.putlabel.argtypes = [c_char_p, c_float, c_float, c_float, c_int, c_int]
    lib.putlabel.restype = None

    lib.SetNameSpace.argtypes = [c_int, c_float]
    lib.SetNameSpace.restype = None

    lib.hardcopy.argtypes = [c_char]
    lib.hardcopy.restype = None
    # --- Full Parity Additions ---
    GPC_FUNQ = ctypes.CFUNCTYPE(c_float, c_float, c_float)

    lib.BoxPlot.argtypes = [c_float, POINTER(c_float), c_int, c_int, c_int, c_float]
    lib.BoxPlot.restype = None

    lib.WaterfallPlot.argtypes = [POINTER(c_float), POINTER(c_float), POINTER(POINTER(c_float)), c_int, c_int]
    lib.WaterfallPlot.restype = None

    lib.WaterfallCurve.argtypes = [POINTER(c_float), c_float, POINTER(c_float), c_int]
    lib.WaterfallCurve.restype = None

    lib.plane3d.argtypes = [c_char, c_float, c_char]
    lib.plane3d.restype = None

    lib.surmat2.argtypes = [POINTER(POINTER(c_float)), c_int, c_int]
    lib.surmat2.restype = None

    lib.trimat.argtypes = [POINTER(c_float), POINTER(c_float), POINTER(c_float), c_int, c_int, c_float, c_float, c_float, POINTER(c_int), c_int]
    lib.trimat.restype = None

    lib.lconmat.argtypes = [POINTER(c_float), POINTER(c_float), c_int, POINTER(c_float), c_int, POINTER(c_float), POINTER(c_int), c_int, c_int, c_char_p]
    lib.lconmat.restype = None

    lib.lcontour.argtypes = [GPC_FUNQ, POINTER(c_float), c_int, POINTER(c_float), c_int, POINTER(c_float), c_int, POINTER(c_int), c_int, c_char_p]
    lib.lcontour.restype = None

    lib.surfun.argtypes = [GPC_FUNQ, c_int, c_float, c_int, c_float]
    lib.surfun.restype = None

    lib.trifun.argtypes = [GPC_FUNQ, c_int, c_float, c_float, c_float, POINTER(c_int), c_int]
    lib.trifun.restype = None

    lib.ColorKeyL.argtypes = [c_float, c_float, POINTER(c_int), c_int, c_float, c_float, c_float, c_float, c_int, c_char_p, c_char, c_float, c_int]
    lib.ColorKeyL.restype = None

    lib.ColorKeyN.argtypes = [POINTER(c_float), POINTER(c_int), c_int, c_float, c_float, c_float, c_float, c_int, c_char_p, c_char, c_float, c_int]
    lib.ColorKeyN.restype = None

    lib.erfcurve.argtypes = [POINTER(c_float), POINTER(c_float), c_int, c_int]
    lib.erfcurve.restype = None

    lib.erfgrid.argtypes = [c_float, c_float, c_float, c_char_p]
    lib.erfgrid.restype = None

    lib.erfscales.argtypes = [c_int, POINTER(c_float), c_int]
    lib.erfscales.restype = None

    lib.scatplot.argtypes = [c_char]
    lib.scatplot.restype = None

    lib.sclfix.argtypes = [c_int]
    lib.sclfix.restype = None

    lib.usertoinch.argtypes = [c_float, c_float, POINTER(c_float), POINTER(c_float)]
    lib.usertoinch.restype = None

    lib.usertopix.argtypes = [c_float, c_float, c_float, POINTER(c_int), POINTER(c_int)]
    lib.usertopix.restype = None

    lib.intopix.argtypes = [c_float]
    lib.intopix.restype = c_int

    lib.pixtoin.argtypes = [c_int]
    lib.pixtoin.restype = c_float

    lib.ellipse.argtypes = [c_float, c_float, c_float, c_float, c_float, c_float, c_float, c_char, c_int]
    lib.ellipse.restype = None

    lib.fillellipse.argtypes = [c_int, c_int, c_int]
    lib.fillellipse.restype = None

    lib.spline.argtypes = [POINTER(c_float), POINTER(c_float), c_int, c_int, c_float, c_float, POINTER(c_float), POINTER(c_float)]
    lib.spline.restype = None

    lib.blank.argtypes = [c_uint]
    lib.blank.restype = None

    lib.wedge.argtypes = [c_float, c_float, c_float, c_float, c_int, c_int]
    lib.wedge.restype = None

    lib.uvector.argtypes = [c_float, c_float, c_float, c_float, c_float, c_float, c_char_p]
    lib.uvector.restype = None

    lib.vector.argtypes = [c_float, c_float, c_float, c_float, c_float, c_float, c_char_p]
    lib.vector.restype = None

    lib.uline.argtypes = [c_float, c_float, c_float, c_float, c_int]
    lib.uline.restype = None

    lib.linch.argtypes = [c_float, c_float, c_float, c_float, c_char]
    lib.linch.restype = None

    lib.line.argtypes = [c_int, c_int, c_int, c_int, c_int]
    lib.line.restype = None

    lib.d3grid.argtypes = [c_int, c_int]
    lib.d3grid.restype = None

    lib.d3pltfnt.argtypes = [c_float, c_float, c_float, c_char_p, c_float, c_int]
    lib.d3pltfnt.restype = None

    lib.HLStoRGB.argtypes = [c_int, c_int, c_int, POINTER(c_int), POINTER(c_int), POINTER(c_int)]
    lib.HLStoRGB.restype = None

    lib.RGBtoHLS.argtypes = [c_int, c_int, c_int, POINTER(c_int), POINTER(c_int), POINTER(c_int)]
    lib.RGBtoHLS.restype = None

    lib.RGBtoCMY.argtypes = [c_int, c_int, c_int, POINTER(c_int), POINTER(c_int), POINTER(c_int)]
    lib.RGBtoCMY.restype = None

    lib.RGBtoCMYK.argtypes = [c_int, c_int, c_int, POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int)]
    lib.RGBtoCMYK.restype = None

    lib.RGBtoIBM.argtypes = [POINTER(c_int)]
    lib.RGBtoIBM.restype = c_int

    lib.RGBcolor.argtypes = [c_int, c_int, c_int]
    lib.RGBcolor.restype = c_int

    lib.mean.argtypes = [POINTER(c_float), c_int]
    lib.mean.restype = c_float

    lib.median.argtypes = [POINTER(c_float), c_int]
    lib.median.restype = c_float

    lib.minmax.argtypes = [POINTER(c_float), POINTER(c_float), POINTER(c_float), c_int]
    lib.minmax.restype = None

    lib.StdDev.argtypes = [POINTER(c_float), c_int]
    lib.StdDev.restype = c_float

    lib.matsolve.argtypes = [POINTER(POINTER(c_double)), c_int, POINTER(c_double)]
    lib.matsolve.restype = None

    lib.matinverse.argtypes = [POINTER(POINTER(c_double)), POINTER(POINTER(c_double)), c_int]
    lib.matinverse.restype = None

    lib.SetTTFull.argtypes = [c_char, c_char]
    lib.SetTTFull.restype = None

    lib.SetTTKerning.argtypes = [c_char, c_char]
    lib.SetTTKerning.restype = None

    lib.SetTTMaxError.argtypes = [c_float, c_char]
    lib.SetTTMaxError.restype = None

    lib.SetTTUseMemory.argtypes = [c_char]
    lib.SetTTUseMemory.restype = None

    lib.settolerance.argtypes = [c_float]
    lib.settolerance.restype = None

    lib.saveplotq.argtypes = []
    lib.saveplotq.restype = None

    lib.rowcolq.argtypes = [c_int, c_int]
    lib.rowcolq.restype = None

    lib.curson.argtypes = [c_int, c_int, POINTER(POINTER(c_float)), POINTER(POINTER(c_float)), POINTER(c_int)]
    lib.curson.restype = None

    lib.curvefollow.argtypes = [c_int, c_int, POINTER(c_float), POINTER(c_float), c_int]
    lib.curvefollow.restype = None

    lib.GetDataPoint.argtypes = [POINTER(c_float), POINTER(c_float)]
    lib.GetDataPoint.restype = c_int

    lib.xfgrid.argtypes = [c_int, c_int]
    lib.xfgrid.restype = None

    lib.yfgrid.argtypes = [c_int, c_int]
    lib.yfgrid.restype = None
    lib.pixtouser.argtypes = [c_int, c_int, POINTER(c_float), POINTER(c_float)]
    lib.pixtouser.restype = None

    lib.rfit.argtypes = [POINTER(c_float), POINTER(c_float), c_int, c_int, c_float, c_int, POINTER(c_float), POINTER(c_float)]
    lib.rfit.restype = None

    lib.gpcBeep.argtypes = [c_int, c_int]
    lib.gpcBeep.restype = None

    lib.set_time.argtypes = [c_int]
    lib.set_time.restype = None

    lib.get_time.argtypes = [c_int]
    lib.get_time.restype = c_char_p

    # Colormap & Continuous Shading
    lib.gpc_rgb.argtypes = [c_int, c_int, c_int]
    lib.gpc_rgb.restype = c_int

    lib.gpc_colormap_eval.argtypes = [c_int, c_float]
    lib.gpc_colormap_eval.restype = c_int

    lib.gpc_get_color.argtypes = [c_int, c_float, c_float, c_float]
    lib.gpc_get_color.restype = c_int

    lib.gpc_get_rgb.argtypes = [c_int, c_float, c_float, c_float, POINTER(c_int), POINTER(c_int), POINTER(c_int)]
    lib.gpc_get_rgb.restype = None

    lib.gpc_scatter.argtypes = [POINTER(c_float), POINTER(c_float), POINTER(c_float), c_int, c_float, c_float, c_int, c_float, c_int]
    lib.gpc_scatter.restype = None

    lib.ColorKeyGradient.argtypes = [c_float, c_float, c_int, c_float, c_float, c_float, c_float, c_int, c_char_p, c_char, c_float, c_int]
    lib.ColorKeyGradient.restype = None

    lib.gpc_load_font.argtypes = [c_char_p, c_char]
    lib.gpc_load_font.restype = None


