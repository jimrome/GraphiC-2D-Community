"""
GraphiC Constants: Colors, Line Styles, and Symbols
"""

# Colors (standard 16-color EGA/VGA palette used by GraphiC)
BLACK = 0
BLUE = 1
GREEN = 2
CYAN = 3
RED = 4
MAGENTA = 5
BROWN = 6
WHITE = 7
DRK_GRAY = 8
LGT_BLUE = 9
LGT_GREEN = 10
LGT_CYAN = 11
LGT_RED = 12
LGT_MAGENTA = 13
YELLOW = 14
BRT_WHITE = 15

# Color name mapping
COLOR_MAP = {
    "black": BLACK,
    "blue": BLUE,
    "green": GREEN,
    "cyan": CYAN,
    "red": RED,
    "magenta": MAGENTA,
    "brown": BROWN,
    "white": WHITE,
    "dark_gray": DRK_GRAY,
    "light_blue": LGT_BLUE,
    "light_green": LGT_GREEN,
    "light_cyan": LGT_CYAN,
    "light_red": LGT_RED,
    "light_magenta": LGT_MAGENTA,
    "yellow": YELLOW,
    "bright_white": BRT_WHITE,
}

# Line Patterns (GraphiC dashf types 1-9)
SOLID = 1
DOTTED = 2
DASHED = 3
LONG_DASH = 4
CHAIN_DASH = 5

PATTERN_MAP = {
    "solid": SOLID,
    "dotted": DOTTED,
    "dashed": DASHED,
    "long_dash": LONG_DASH,
    "chain_dash": CHAIN_DASH,
}

# Plot Symbols (GraphiC symbol indices 0-14)
SYM_SQUARE = 0
SYM_CIRCLE = 1
SYM_DEL = 2
SYM_PLUS = 3
SYM_CROSS = 4
SYM_DIAMOND = 5
SYM_TRIANGLE = 6
SYM_STICK_FIGURE = 7
SYM_FEET_PLUS = 8
SYM_HLINE = 9
SYM_VLINE = 10
SYM_BOX = 11
SYM_DOT = 12
SYM_BIG_DOT = 13
SYM_BIGGER_DOT = 14

# Scientific Colormaps
COLORMAP_VIRIDIS = 0
COLORMAP_PLASMA = 1
COLORMAP_INFERNO = 2
COLORMAP_MAGMA = 3
COLORMAP_COOLWARM = 4
COLORMAP_RAINBOW = 5
COLORMAP_TURBO = 6

