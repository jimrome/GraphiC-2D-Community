package com.sciend.graphic;

/**
 * Marker symbols supported by GraphiC sympick (0-14).
 */
public enum Symbol {
    SQUARE(0),
    CIRCLE(1),
    DEL(2),
    PLUS(3),
    CROSS(4),
    DIAMOND(5),
    TRIANGLE(6),
    STICK_FIGURE(7),
    FEET_PLUS(8),
    HLINE(9),
    VLINE(10),
    BOX(11),
    DOT(12),
    BIG_DOT(13),
    BIGGER_DOT(14);

    private final int code;

    Symbol(int code) {
        this.code = code;
    }

    public int getCode() {
        return code;
    }
}
