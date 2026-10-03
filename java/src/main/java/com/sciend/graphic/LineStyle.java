package com.sciend.graphic;

/**
 * Line styles supported by GraphiC dashf (1-9).
 */
public enum LineStyle {
    SOLID(1),
    DOTTED(2),
    DASHED(3),
    LONG_DASH(4),
    CHAIN_DASH(5);

    private final int code;

    LineStyle(int code) {
        this.code = code;
    }

    public int getCode() {
        return code;
    }
}
