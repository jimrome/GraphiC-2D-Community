"""
GraphiC: Publication-Quality Scientific Graphics Library
"""

from .figure import Figure
from .procedural import (
    bgnplot, page, startplot, area2d, svgcrop, svg_crop, svgopen, svg_open, svgpush, svg_push,
    physor, orel, metricunits,
    box, pbox, heading, xname, yname, zname,
    color, tcolor, bcolor, dashf, sympick, symbol,
    graf, scales, curve, endplot, stopplot,
    font, skew, widen, supsym, subsym, tfont, tifont,
    get_color, get_rgb, colormap_eval, scatter, colorkey_gradient,
    d3bars, plane3d, waterfall_plot, WaterfallPlot, waterfall_curve, WaterfallCurve
)
from .constants import (
    BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, WHITE,
    DRK_GRAY, LGT_BLUE, LGT_GREEN, LGT_CYAN, LGT_RED, LGT_MAGENTA,
    YELLOW, BRT_WHITE,
    SOLID, DOTTED, DASHED, LONG_DASH, CHAIN_DASH,
    SYM_SQUARE, SYM_CIRCLE, SYM_DEL, SYM_PLUS, SYM_CROSS,
    SYM_DIAMOND, SYM_TRIANGLE, SYM_STICK_FIGURE, SYM_FEET_PLUS,
    SYM_HLINE, SYM_VLINE, SYM_BOX, SYM_DOT, SYM_BIG_DOT, SYM_BIGGER_DOT
)

__version__ = "7.0.0"
__all__ = [
    "Figure",
    "bgnplot", "page", "startplot", "area2d", "svgcrop", "svg_crop", "svgopen", "svg_open", "svgpush", "svg_push",
    "physor", "orel", "metricunits",
    "box", "pbox", "heading", "xname", "yname", "zname",
    "color", "tcolor", "bcolor", "dashf", "sympick", "symbol",
    "graf", "scales", "curve", "endplot", "stopplot",
    "font", "skew", "widen", "supsym", "subsym", "tfont", "tifont",
    "get_color", "get_rgb", "colormap_eval", "scatter", "colorkey_gradient",
    "d3bars", "plane3d", "waterfall_plot", "WaterfallPlot", "waterfall_curve", "WaterfallCurve",
    "BLACK", "BLUE", "GREEN", "CYAN", "RED", "MAGENTA", "BROWN", "WHITE",
    "DRK_GRAY", "LGT_BLUE", "LGT_GREEN", "LGT_CYAN", "LGT_RED", "LGT_MAGENTA",
    "YELLOW", "BRT_WHITE",
    "SOLID", "DOTTED", "DASHED", "LONG_DASH", "CHAIN_DASH",
    "SYM_SQUARE", "SYM_CIRCLE", "SYM_DEL", "SYM_PLUS", "SYM_CROSS",
    "SYM_DIAMOND", "SYM_TRIANGLE", "SYM_STICK_FIGURE", "SYM_FEET_PLUS",
    "SYM_HLINE", "SYM_VLINE", "SYM_BOX", "SYM_DOT", "SYM_BIG_DOT", "SYM_BIGGER_DOT"
]

