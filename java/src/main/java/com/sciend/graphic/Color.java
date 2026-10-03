package com.sciend.graphic;

/**
 * Standard 16-color palette supported by GraphiC.
 */
public enum Color {
    BLACK(0),
    BLUE(1),
    GREEN(2),
    CYAN(3),
    RED(4),
    MAGENTA(5),
    BROWN(6),
    WHITE(7),
    DRK_GRAY(8),
    LGT_BLUE(9),
    LGT_GREEN(10),
    LGT_CYAN(11),
    LGT_RED(12),
    LGT_MAGENTA(13),
    YELLOW(14),
    BRT_WHITE(15);

    private final int code;

    Color(int code) {
        this.code = code;
    }

    public int getCode() {
        return code;
    }
}
