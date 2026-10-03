package com.sciend.graphic;

/**
 * Functional interface for two-variable floating-point functions f(x, y),
 * used in mathematical surfaces (surfun) and ternary contour maps (trifun).
 */
@FunctionalInterface
public interface BinaryFloatFunction {
    float apply(float x, float y);
}
