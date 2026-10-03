package com.sciend.graphic;

import java.lang.foreign.Arena;
import java.lang.foreign.FunctionDescriptor;
import java.lang.foreign.Linker;
import java.lang.foreign.MemorySegment;
import java.lang.foreign.ValueLayout;
import java.lang.invoke.MethodHandle;
import java.lang.invoke.MethodHandles;
import java.lang.invoke.MethodType;

/**
 * Authentic procedural state machine API for GraphiC in Java 22 FFM.
 * Provides direct 1:1 parity with classic C and Fortran GraphiC routine names:
 * bgnplot -> page -> startplot -> area2d/physor -> box -> heading/xname/yname -> graf -> curve -> endplot/stopplot
 */
public class Graphic {

    // Color constants
    public static final Color BLACK = Color.BLACK;
    public static final Color BLUE = Color.BLUE;
    public static final Color GREEN = Color.GREEN;
    public static final Color CYAN = Color.CYAN;
    public static final Color RED = Color.RED;
    public static final Color MAGENTA = Color.MAGENTA;
    public static final Color BROWN = Color.BROWN;
    public static final Color WHITE = Color.WHITE;
    public static final Color DRK_GRAY = Color.DRK_GRAY;
    public static final Color LGT_BLUE = Color.LGT_BLUE;
    public static final Color LGT_GREEN = Color.LGT_GREEN;
    public static final Color LGT_CYAN = Color.LGT_CYAN;
    public static final Color LGT_RED = Color.LGT_RED;
    public static final Color LGT_MAGENTA = Color.LGT_MAGENTA;
    public static final Color YELLOW = Color.YELLOW;
    public static final Color BRT_WHITE = Color.BRT_WHITE;

    // Line style constants
    public static final LineStyle SOLID = LineStyle.SOLID;
    public static final LineStyle DOTTED = LineStyle.DOTTED;
    public static final LineStyle DASHED = LineStyle.DASHED;
    public static final LineStyle LONG_DASH = LineStyle.LONG_DASH;
    public static final LineStyle CHAIN_DASH = LineStyle.CHAIN_DASH;

    // Scientific Colormaps
    public static final int COLORMAP_VIRIDIS = 0;
    public static final int COLORMAP_PLASMA = 1;
    public static final int COLORMAP_INFERNO = 2;
    public static final int COLORMAP_MAGMA = 3;
    public static final int COLORMAP_COOLWARM = 4;
    public static final int COLORMAP_RAINBOW = 5;
    public static final int COLORMAP_TURBO = 6;

    public static void bgnplot(int mode, char driver, String filename) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment seg = arena.allocateFrom(filename);
            GraphicNative.bgnplot.invokeExact(mode, (int) driver, seg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void page(float width, float height) {
        try {
            GraphicNative.page.invokeExact(width, height);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void startplot(int bgcolor) {
        try {
            GraphicNative.startplot.invokeExact(bgcolor);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void startplot(Color bgcolor) {
        startplot(bgcolor.getCode());
    }

    public static void area2d(float xlen, float ylen) {
        try {
            GraphicNative.area2d.invokeExact(xlen, ylen);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void physor(float x, float y) {
        try {
            GraphicNative.physor.invokeExact(x, y);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void orel(float dx, float dy) {
        try {
            GraphicNative.orel.invokeExact(dx, dy);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void box() {
        try {
            GraphicNative.box.invokeExact();
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void pbox() {
        try {
            GraphicNative.pbox.invokeExact();
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void heading(String title) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment seg = arena.allocateFrom(title);
            GraphicNative.heading.invokeExact(seg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void xname(String label) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment seg = arena.allocateFrom(label);
            GraphicNative.xname.invokeExact(seg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void yname(String label) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment seg = arena.allocateFrom(label);
            GraphicNative.yname.invokeExact(seg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void zname(String label) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment seg = arena.allocateFrom(label);
            GraphicNative.z3name.invokeExact(seg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void color(int colorCode) {
        try {
            GraphicNative.color.invokeExact(colorCode);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void color(Color color) {
        color(color.getCode());
    }

    public static void dashf(int pattern) {
        try {
            GraphicNative.dashf.invokeExact(pattern);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void dashf(LineStyle style) {
        dashf(style.getCode());
    }

    public static void sympick(int symbol) {
        try {
            GraphicNative.sympick.invokeExact(symbol);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void sympick(Symbol symbol) {
        sympick(symbol.getCode());
    }

    public static void graf(String xfmt, float xmin, float xstep, float xmax,
                            String yfmt, float ymin, float ystep, float ymax) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xf = arena.allocateFrom(xfmt);
            MemorySegment yf = arena.allocateFrom(yfmt);
            GraphicNative.graf.invokeExact(xf, xmin, xstep, xmax, yf, ymin, ystep, ymax);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void curve(float[] x, float[] y, int n, int symbol) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            GraphicNative.curve.invokeExact(xSeg, ySeg, n, symbol);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void curve(float[] x, float[] y) {
        curve(x, y, Math.min(x.length, y.length), 0);
    }

    public static int endplot() {
        try {
            return (int) GraphicNative.endplot.invokeExact();
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void stopplot() {
        try {
            GraphicNative.stopplot.invokeExact();
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void box3d() {
        try {
            GraphicNative.box3d.invokeExact();
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void cross(int iscross) {
        try {
            GraphicNative.cross.invokeExact((byte) iscross);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void CrossAt(float x, float y) {
        try {
            GraphicNative.CrossAt.invokeExact(x, y);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void frame(int frameon, int ftick) {
        try {
            GraphicNative.frame.invokeExact((byte) frameon, (byte) ftick);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void tickht(float height) {
        try {
            GraphicNative.tickht.invokeExact(height);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void tickout(int is_out) {
        try {
            GraphicNative.tickout.invokeExact(is_out);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void numht(float height) {
        try {
            GraphicNative.numht.invokeExact(height);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void titlht(float height) {
        try {
            GraphicNative.titlht.invokeExact(height);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void axnamht(float height) {
        try {
            GraphicNative.axnamht.invokeExact(height);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void upright(int uplab) {
        try {
            GraphicNative.upright.invokeExact((byte) uplab);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void acrop(int flag) {
        try {
            GraphicNative.acrop.invokeExact((byte) flag);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void svgcrop(float marginInches) {
        try {
            GraphicNative.svgcrop.invokeExact(marginInches);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void svgCrop(float marginInches) {
        svgcrop(marginInches);
    }

    public static int svgopen(String filepath) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment p = (filepath != null) ? arena.allocateFrom(filepath) : MemorySegment.NULL;
            return (int) GraphicNative.svgopen.invokeExact(p);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static int svgopen() {
        return svgopen(null);
    }

    public static int svg_open(String filepath) {
        return svgopen(filepath);
    }

    public static int svg_open() {
        return svgopen(null);
    }

    public static int svgpush(String url, String filepath) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment u = (url != null) ? arena.allocateFrom(url) : MemorySegment.NULL;
            MemorySegment f = (filepath != null) ? arena.allocateFrom(filepath) : MemorySegment.NULL;
            return (int) GraphicNative.svgpush.invokeExact(u, f);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static int svgpush(String url) {
        return svgpush(url, null);
    }

    public static int svg_push(String url, String filepath) {
        return svgpush(url, filepath);
    }

    public static int svg_push(String url) {
        return svgpush(url, null);
    }

    public static void rotate(int isrot) {
        try {
            GraphicNative.rotate.invokeExact((byte) isrot);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void pgshift(float x, float y) {
        try {
            GraphicNative.pgshift.invokeExact(x, y);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void fgrid(int fon, int ndiv) {
        try {
            GraphicNative.fgrid.invokeExact(fon, ndiv);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void xfgrid(int fon, int nx) {
        try {
            GraphicNative.xfgrid.invokeExact(fon, nx);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void yfgrid(int fon, int ny) {
        try {
            GraphicNative.yfgrid.invokeExact(fon, ny);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void d3line(float x1, float y1, float z1, float x2, float y2, float z2) {
        try {
            GraphicNative.d3line.invokeExact(x1, y1, z1, x2, y2, z2);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void d3color(int[] colors, int nlevels, int mesh) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment cSeg = arena.allocateFrom(ValueLayout.JAVA_INT, colors);
            GraphicNative.d3color.invokeExact(cSeg, nlevels, (byte) mesh);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void sclfix(int isfixed) {
        try {
            GraphicNative.sclfix.invokeExact(isfixed);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void nohide(int visible) {
        try {
            GraphicNative.nohide.invokeExact(visible);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void vuangl(float phi, float theta, float radius) {
        try {
            GraphicNative.vuangl.invokeExact(phi, theta, radius);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void contcolor(int[] colors, int border) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment cSeg = arena.allocateFrom(ValueLayout.JAVA_INT, colors);
            GraphicNative.contcolor_fixed.invokeExact(cSeg, border, MemorySegment.NULL);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void ctline(String text, float height) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment strSeg = arena.allocateFrom(text);
            GraphicNative.ctline.invokeExact(strSeg, height);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void ltline(String text, float height) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment strSeg = arena.allocateFrom(text);
            GraphicNative.ltline.invokeExact(strSeg, height);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void rtline(String text, float height) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment strSeg = arena.allocateFrom(text);
            GraphicNative.rtline.invokeExact(strSeg, height);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static float strwidth(String text, float height) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment strSeg = arena.allocateFrom(text);
            return (float) GraphicNative.strwidth.invokeExact(strSeg, height);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void subsym(char ch) {
        try {
            GraphicNative.subsym.invokeExact((byte) ch);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void legend(int marker, String text, int style, float height, int color) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment strSeg = arena.allocateFrom(text);
            GraphicNative.legend.invokeExact(marker, strSeg, style, height, color);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void legpos(int entries, float x, float y, int border) {
        try {
            GraphicNative.legpos.invokeExact(entries, x, y, border);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void loglog(float xorig, float xmax, float yorig, float ymax) {
        try {
            GraphicNative.loglog.invokeExact(xorig, xmax, yorig, ymax);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void tline(int width) {
        try {
            GraphicNative.tline.invokeExact(width);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void tcurve(int width) {
        try {
            GraphicNative.tcurve.invokeExact(width);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void plotadd() {
        try {
            GraphicNative.plotadd.invokeExact();
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void dateit(char font) {
        try {
            GraphicNative.dateit.invokeExact((byte) font);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    // Hershey font state registers
    public static void linesp(float ratio) {
        try {
            GraphicNative.linesp.invokeExact(ratio);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void lmargin(float marginch) {
        try {
            GraphicNative.lmargin.invokeExact(marginch);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void rmargin(float marginch) {
        try {
            GraphicNative.rmargin.invokeExact(marginch);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void tmargin(float marginch) {
        try {
            GraphicNative.tmargin.invokeExact(marginch);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void charspc(int width) {
        try {
            GraphicNative.charspc.invokeExact(width);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void skew(float slant) {
        try {
            GraphicNative.skew.invokeExact(slant);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void widen(float fact) {
        try {
            GraphicNative.widen.invokeExact(fact);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void tfont(int wid) {
        try {
            GraphicNative.tfont.invokeExact(wid);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void tifont(float wid) {
        try {
            GraphicNative.tifont.invokeExact(wid);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void fntchg(char sym) {
        try {
            GraphicNative.fntchg.invokeExact((byte) sym);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void supsym(char ch) {
        try {
            GraphicNative.supsym.invokeExact((byte) ch);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void putlabel(String text, float xpos, float ypos, float height, int border, int units) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment strSeg = arena.allocateFrom(text);
            GraphicNative.putlabel.invokeExact(strSeg, xpos, ypos, height, border, units);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void ylog(String format, float xorig, float xstep, float xmax, float yorig, float ymax) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment fmtSeg = arena.allocateFrom(format);
            GraphicNative.ylog.invokeExact(fmtSeg, xorig, xstep, xmax, yorig, ymax);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void prtfnt(float xinch, float yinch, String text, float height, int angle) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment strSeg = arena.allocateFrom(text);
            GraphicNative.prtfnt.invokeExact(xinch, yinch, strSeg, height, angle);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void setNameSpace(int axis, float factor) {
        try {
            GraphicNative.SetNameSpace.invokeExact(axis, factor);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void hardcopy(byte type) {
        try {
            GraphicNative.hardcopy.invokeExact(type);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void metricunits(int flag) {
        try {
            GraphicNative.metricunits.invokeExact(flag);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void tcolor(int val) {
        try {
            GraphicNative.tcolor.invokeExact(val);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void bcolor(int val) {
        try {
            GraphicNative.bcolor.invokeExact(val);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void x3name(String text) {
        try (Arena arena = Arena.ofConfined()) {
            GraphicNative.x3name.invokeExact(arena.allocateFrom(text));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void y3name(String text) {
        try (Arena arena = Arena.ofConfined()) {
            GraphicNative.y3name.invokeExact(arena.allocateFrom(text));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void z3name(String text) {
        try (Arena arena = Arena.ofConfined()) {
            GraphicNative.z3name.invokeExact(arena.allocateFrom(text));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void d3head(String text, int font) {
        try (Arena arena = Arena.ofConfined()) {
            GraphicNative.d3head.invokeExact(arena.allocateFrom(text), font);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void scales(int type, int code, float[] x, float[] y, int n) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xSeg = (x != null) ? arena.allocateFrom(ValueLayout.JAVA_FLOAT, x) : MemorySegment.NULL;
            MemorySegment ySeg = (y != null) ? arena.allocateFrom(ValueLayout.JAVA_FLOAT, y) : MemorySegment.NULL;
            GraphicNative.scales.invokeExact(type, code, xSeg, ySeg, n);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void volm3d(float xlen, float ylen, float zlen) {
        try {
            GraphicNative.volm3d.invokeExact(xlen, ylen, zlen);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void vuabs(float r, float theta, float phi) {
        try {
            GraphicNative.vuabs.invokeExact(r, theta, phi);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void graf3d(float xmin, float xstep, float xmax, float ymin, float ystep, float ymax, float zmin, float zstep, float zmax) {
        try {
            GraphicNative.graf3d.invokeExact(xmin, xstep, xmax, ymin, ystep, ymax, zmin, zstep, zmax);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void survis(float[] angles) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment seg = (angles != null) ? arena.allocateFrom(ValueLayout.JAVA_FLOAT, angles) : MemorySegment.NULL;
            GraphicNative.survis.invokeExact(seg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void d3base(int isbase, int colorb) {
        try {
            GraphicNative.d3base.invokeExact(isbase, colorb);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void surmat(float[] z, int nx, int ny) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment seg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, z);
            GraphicNative.surmat.invokeExact(seg, nx, ny);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void conmat(float[] z, float[] x, int nx, float[] y, int ny, float zmin, float zstep, float zmax, int labelInterval) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment zSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, z);
            MemorySegment xSeg = (x != null) ? arena.allocateFrom(ValueLayout.JAVA_FLOAT, x) : MemorySegment.NULL;
            MemorySegment ySeg = (y != null) ? arena.allocateFrom(ValueLayout.JAVA_FLOAT, y) : MemorySegment.NULL;
            GraphicNative.conmat.invokeExact(zSeg, xSeg, nx, ySeg, ny, zmin, zstep, zmax, MemorySegment.NULL, labelInterval);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void smdraw(int smon) {
        try {
            GraphicNative.smdraw.invokeExact(smon);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void plotrx(float xlen, float ylen) {
        try {
            GraphicNative.plotrx.invokeExact(xlen, ylen);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void panelinch(float[] x, float[] y, int n, int fill, int border) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            GraphicNative.panelinch.invokeExact(xSeg, ySeg, n, fill, border);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void panproc() {
        try {
            GraphicNative.panproc.invokeExact();
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void bar(int n, float[] x, float[] y, int type, float width, int fill, int[] colors, int[] borders) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            MemorySegment cSeg = (colors != null) ? arena.allocateFrom(ValueLayout.JAVA_INT, colors) : MemorySegment.NULL;
            MemorySegment bSeg = (borders != null) ? arena.allocateFrom(ValueLayout.JAVA_INT, borders) : MemorySegment.NULL;
            GraphicNative.bar.invokeExact(n, xSeg, ySeg, type, width, fill, cSeg, bSeg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void pieplot(int dim, float[] values, int[] colors, float[] offsets, String[] captions, float size, int border, int font) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment valSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, values);
            MemorySegment clrSeg = (colors != null) ? arena.allocateFrom(ValueLayout.JAVA_INT, colors) : MemorySegment.NULL;
            MemorySegment offSeg = (offsets != null) ? arena.allocateFrom(ValueLayout.JAVA_FLOAT, offsets) : MemorySegment.NULL;
            MemorySegment capArraySeg = MemorySegment.NULL;
            if (captions != null && captions.length > 0) {
                int cDim = Math.min(dim, captions.length);
                MemorySegment[] segs = new MemorySegment[cDim];
                for (int i = 0; i < cDim; i++) segs[i] = arena.allocateFrom(captions[i]);
                capArraySeg = arena.allocate(ValueLayout.ADDRESS, cDim);
                for (int i = 0; i < cDim; i++) capArraySeg.setAtIndex(ValueLayout.ADDRESS, i, segs[i]);
            }
            GraphicNative.pieplot.invokeExact(dim, valSeg, clrSeg, offSeg, capArraySeg, size, border, font);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void polgrid(float rmax, float rmin, int mode, String[] labels, int nray, byte col) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment labArraySeg = MemorySegment.NULL;
            int labNum = 0;
            if (labels != null && labels.length > 0) {
                labNum = labels.length;
                MemorySegment[] segs = new MemorySegment[labNum];
                for (int i = 0; i < labNum; i++) segs[i] = arena.allocateFrom(labels[i]);
                labArraySeg = arena.allocate(ValueLayout.ADDRESS, labNum);
                for (int i = 0; i < labNum; i++) labArraySeg.setAtIndex(ValueLayout.ADDRESS, i, segs[i]);
            }
            GraphicNative.polgrid.invokeExact(rmax, rmin, mode, labArraySeg, labNum, nray, col);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void polcurve(float[] r, float[] theta, int mark) {
        try (Arena arena = Arena.ofConfined()) {
            int n = Math.min(r.length, theta.length);
            MemorySegment rSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, r);
            MemorySegment thSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, theta);
            GraphicNative.polcurve.invokeExact(rSeg, thSeg, n, mark);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void xlog(float xmin, float xmax, String form, float yorig, float ystep, float ymax) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment formSeg = arena.allocateFrom(form);
            GraphicNative.xlog.invokeExact(xmin, xmax, formSeg, yorig, ystep, ymax);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void lglgscle(float[] x, float[] y) {
        try (Arena arena = Arena.ofConfined()) {
            int n = Math.min(x.length, y.length);
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            GraphicNative.lglgscle.invokeExact(xSeg, ySeg, n);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void uvector(float x1, float y1, float x2, float y2, float headAngle, float headSize, String opt) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment optSeg = arena.allocateFrom(opt);
            GraphicNative.uvector.invokeExact(x1, y1, x2, y2, headAngle, headSize, optSeg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void d3bars(float bwid) {
        try {
            GraphicNative.d3bars.invokeExact(bwid);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void aname(String text) {
        try (Arena arena = Arena.ofConfined()) {
            GraphicNative.aname.invokeExact(arena.allocateFrom(text));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void bname(String text) {
        try (Arena arena = Arena.ofConfined()) {
            GraphicNative.bname.invokeExact(arena.allocateFrom(text));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void cname(String text) {
        try (Arena arena = Arena.ofConfined()) {
            GraphicNative.cname.invokeExact(arena.allocateFrom(text));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void simuplot(int nplots) {
        try {
            GraphicNative.simuplot.invokeExact(nplots);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void context(int icontext) {
        try {
            GraphicNative.context.invokeExact(icontext);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void axesoff(int bits) {
        try {
            GraphicNative.axesoff.invokeExact(bits);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void pltfnt(float x, float y, String text, float size, int angle) {
        try (Arena arena = Arena.ofConfined()) {
            GraphicNative.pltfnt.invokeExact(x, y, arena.allocateFrom(text), size, angle);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void curv3d(float[] x, float[] y, float[] z, int n) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            MemorySegment zSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, z);
            GraphicNative.curv3d.invokeExact(xSeg, ySeg, zSeg, n);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void contcolor(int[] colors, int border, int[] styles) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment cSeg = (colors != null) ? arena.allocateFrom(ValueLayout.JAVA_INT, colors) : MemorySegment.NULL;
            MemorySegment sSeg = (styles != null) ? arena.allocateFrom(ValueLayout.JAVA_INT, styles) : MemorySegment.NULL;
            GraphicNative.contcolor_fixed.invokeExact(cSeg, border, sSeg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void curvfill(float[] x1, float[] y1, int n1, float[] x2, float[] y2, int n2, int color, int pattern) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment x1Seg = (x1 != null) ? arena.allocateFrom(ValueLayout.JAVA_FLOAT, x1) : MemorySegment.NULL;
            MemorySegment y1Seg = (y1 != null) ? arena.allocateFrom(ValueLayout.JAVA_FLOAT, y1) : MemorySegment.NULL;
            MemorySegment x2Seg = (x2 != null) ? arena.allocateFrom(ValueLayout.JAVA_FLOAT, x2) : MemorySegment.NULL;
            MemorySegment y2Seg = (y2 != null) ? arena.allocateFrom(ValueLayout.JAVA_FLOAT, y2) : MemorySegment.NULL;
            GraphicNative.curvfill.invokeExact(x1Seg, y1Seg, n1, x2Seg, y2Seg, n2, color, pattern);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void errorbar(float x, float y, float dx1, float dx2, float dy1, float dy2) {
        try {
            GraphicNative.errorbar.invokeExact(x, y, dx1, dx2, dy1, dy2);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void scatplot(char type) {
        try {
            GraphicNative.scatplot.invokeExact((byte) type);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void histogram(float[] x, float[] y, int n, int fill, int[] colors) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            MemorySegment cSeg = (colors != null) ? arena.allocateFrom(ValueLayout.JAVA_INT, colors) : MemorySegment.NULL;
            GraphicNative.histogram.invokeExact(xSeg, ySeg, n, fill, cSeg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void wedge(float rmin, float rmax, float thmin, float thmax, int color, int pattern) {
        try {
            GraphicNative.wedge.invokeExact(rmin, rmax, thmin, thmax, color, pattern);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void stack(char mode) {
        try {
            GraphicNative.stack.invokeExact((byte) mode);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void plotrho(float xlen, float ylen) {
        try {
            GraphicNative.plotrho.invokeExact(xlen, ylen);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void smithcir(float r, float x, float rad) {
        try {
            GraphicNative.smithcir.invokeExact(r, x, rad);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void smove(float angle) {
        try {
            GraphicNative.smove.invokeExact(angle);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void plotabsolute(int flag) {
        try {
            GraphicNative.plotabsolute.invokeExact(flag);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void newFile(String filename) {
        try (Arena arena = Arena.ofConfined()) {
            GraphicNative.NewFile.invokeExact(arena.allocateFrom(filename));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void NewFile(String filename) {
        newFile(filename);
    }

    public static void fillfont(int fill) {
        try {
            GraphicNative.fillfont.invokeExact(fill);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void fontfill(int fill, int pattern, int border) {
        try {
            GraphicNative.fontfill.invokeExact(fill, pattern, border);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    // --- Full Parity Additions for Graphic ---

    public static void boxPlot(float x, float[] y, int outlier, int symnum, float width) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            GraphicNative.BoxPlot.invokeExact(x, ySeg, y.length, outlier, symnum, width);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void waterfallPlot(float[] x, float[] y, float[][] z) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            int ny = y.length;
            MemorySegment rowPtrs = arena.allocate(ValueLayout.ADDRESS, ny);
            for (int i = 0; i < ny; i++) {
                MemorySegment rSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, z[i]);
                rowPtrs.setAtIndex(ValueLayout.ADDRESS, i, rSeg);
            }
            GraphicNative.WaterfallPlot.invokeExact(xSeg, ySeg, rowPtrs, x.length, ny);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void WaterfallPlot(float[] x, float[] y, float[][] z) {
        waterfallPlot(x, y, z);
    }

    public static void waterfallCurve(float[] x, float y, float[] z) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment zSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, z);
            GraphicNative.WaterfallCurve.invokeExact(xSeg, y, zSeg, x.length);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void WaterfallCurve(float[] x, float y, float[] z) {
        waterfallCurve(x, y, z);
    }

    public static void plane3d(char wplane, float pos, char hide) {
        try {
            GraphicNative.plane3d.invokeExact((byte) wplane, pos, (byte) hide);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void colorKeyL(float minv, float maxv, int[] intensity, float legllx, float leglly, float legurx, float legury, int labint, String format, char fontc, float height, int angle) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment intSeg = arena.allocateFrom(ValueLayout.JAVA_INT, intensity);
            MemorySegment fmtSeg = arena.allocateFrom(format);
            GraphicNative.ColorKeyL.invokeExact(minv, maxv, intSeg, intensity.length, legllx, leglly, legurx, legury, labint, fmtSeg, (byte) fontc, height, angle);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void erfcurve(float[] x, float[] y, int sym) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            int n = Math.min(x.length, y.length);
            GraphicNative.erfcurve.invokeExact(xSeg, ySeg, n, sym);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void erfgrid(float ymin, float ystep, float ymax, String yformat) {
        try (Arena arena = Arena.ofConfined()) {
            GraphicNative.erfgrid.invokeExact(ymin, ystep, ymax, arena.allocateFrom(yformat));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void erfscales(int nydiv, float[] y) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            GraphicNative.erfscales.invokeExact(nydiv, ySeg, y.length);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static float[] usertoinch(float x, float y) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xo = arena.allocate(ValueLayout.JAVA_FLOAT);
            MemorySegment yo = arena.allocate(ValueLayout.JAVA_FLOAT);
            GraphicNative.usertoinch.invokeExact(x, y, xo, yo);
            return new float[] { xo.get(ValueLayout.JAVA_FLOAT, 0), yo.get(ValueLayout.JAVA_FLOAT, 0) };
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static int[] usertopix(float x, float y, float z) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xo = arena.allocate(ValueLayout.JAVA_INT);
            MemorySegment yo = arena.allocate(ValueLayout.JAVA_INT);
            GraphicNative.usertopix.invokeExact(x, y, z, xo, yo);
            return new int[] { xo.get(ValueLayout.JAVA_INT, 0), yo.get(ValueLayout.JAVA_INT, 0) };
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static int intopix(float inches) {
        try {
            return (int) GraphicNative.intopix.invokeExact(inches);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static float pixtoin(int pixels) {
        try {
            return (float) GraphicNative.pixtoin.invokeExact(pixels);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void ellipse(float xc, float yc, float xr, float yr, float rotation, float th1, float th2, boolean degrees, int units) {
        try {
            GraphicNative.ellipse.invokeExact(xc, yc, xr, yr, rotation, th1, th2, (byte)(degrees ? 1 : 0), units);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void fillellipse(int onoff, int pattern, int border) {
        try {
            GraphicNative.fillellipse.invokeExact(onoff, pattern, border);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void blank(int attr) {
        try {
            GraphicNative.blank.invokeExact(attr);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void d3grid(int subdiv, int label) {
        try {
            GraphicNative.d3grid.invokeExact(subdiv, label);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void d3pltfnt(float x, float y, float z, String string, float height, int angle) {
        try (Arena arena = Arena.ofConfined()) {
            GraphicNative.d3pltfnt.invokeExact(x, y, z, arena.allocateFrom(string), height, angle);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static int[] hlstoRGB(int h, int l, int s) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment r = arena.allocate(ValueLayout.JAVA_INT);
            MemorySegment g = arena.allocate(ValueLayout.JAVA_INT);
            MemorySegment b = arena.allocate(ValueLayout.JAVA_INT);
            GraphicNative.HLStoRGB.invokeExact(h, l, s, r, g, b);
            return new int[] { r.get(ValueLayout.JAVA_INT, 0), g.get(ValueLayout.JAVA_INT, 0), b.get(ValueLayout.JAVA_INT, 0) };
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static int[] rgbtoHLS(int r, int g, int b) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment h = arena.allocate(ValueLayout.JAVA_INT);
            MemorySegment l = arena.allocate(ValueLayout.JAVA_INT);
            MemorySegment s = arena.allocate(ValueLayout.JAVA_INT);
            GraphicNative.RGBtoHLS.invokeExact(r, g, b, h, l, s);
            return new int[] { h.get(ValueLayout.JAVA_INT, 0), l.get(ValueLayout.JAVA_INT, 0), s.get(ValueLayout.JAVA_INT, 0) };
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static float mean(float[] x) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            return (float) GraphicNative.mean.invokeExact(xSeg, x.length);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static float median(float[] x) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            return (float) GraphicNative.median.invokeExact(xSeg, x.length);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static float stdDev(float[] x) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            return (float) GraphicNative.StdDev.invokeExact(xSeg, x.length);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static float stddev(float[] x) {
        return stdDev(x);
    }

    public static float StdDev(float[] x) {
        return stdDev(x);
    }

    public static void setTTFull(boolean onoff, boolean string) {
        try {
            GraphicNative.SetTTFull.invokeExact((byte)(onoff ? 1 : 0), (byte)(string ? 1 : 0));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void setTTKerning(boolean onoff, boolean string) {
        try {
            GraphicNative.SetTTKerning.invokeExact((byte)(onoff ? 1 : 0), (byte)(string ? 1 : 0));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void setTTMaxError(float err, boolean string) {
        try {
            GraphicNative.SetTTMaxError.invokeExact(err, (byte)(string ? 1 : 0));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void setTTUseMemory(boolean onoff) {
        try {
            GraphicNative.SetTTUseMemory.invokeExact((byte)(onoff ? 1 : 0));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void settolerance(float tol) {
        try {
            GraphicNative.settolerance.invokeExact(tol);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void saveplotq() {
        try {
            GraphicNative.saveplotq.invokeExact();
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }


    public static float[] pixtouser(int xin, int yin) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment xo = arena.allocate(ValueLayout.JAVA_FLOAT);
            MemorySegment yo = arena.allocate(ValueLayout.JAVA_FLOAT);
            GraphicNative.pixtouser.invokeExact(xin, yin, xo, yo);
            return new float[] { xo.get(ValueLayout.JAVA_FLOAT, 0), yo.get(ValueLayout.JAVA_FLOAT, 0) };
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void gpcBeep(int freq, int duration) {
        try {
            GraphicNative.gpcBeep.invokeExact(freq, duration);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void setTime(int timerId) {
        try {
            GraphicNative.set_time.invokeExact(timerId);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public static void trimat(float[] m1, float[] m2, float[] z, int dim1,
                              float zmin, float zstep, float zmax, int[] lnstyle, int border) {
        try (Arena arena = Arena.ofConfined()) {
            int dim = Math.min(Math.min(m1.length, m2.length), z.length);
            MemorySegment m1Seg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, m1);
            MemorySegment m2Seg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, m2);
            MemorySegment zSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, z);
            MemorySegment lnSeg = arena.allocateFrom(ValueLayout.JAVA_INT, lnstyle);
            GraphicNative.trimat.invokeExact(m1Seg, m2Seg, zSeg, dim, dim1, zmin, zstep, zmax, lnSeg, border);
        } catch (Throwable t) {
            throw new RuntimeException("trimat failed", t);
        }
    }

    public static void trifun(BinaryFloatFunction fun, int dim, float zmin, float zstep, float zmax, int[] lnstyle, int border) {
        try (Arena arena = Arena.ofConfined()) {
            MethodHandle target = MethodHandles.lookup().findVirtual(
                BinaryFloatFunction.class, "apply",
                MethodType.methodType(float.class, float.class, float.class)
            ).bindTo(fun);
            MemorySegment stub = Linker.nativeLinker().upcallStub(
                target,
                FunctionDescriptor.of(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT),
                arena
            );
            MemorySegment lnSeg = arena.allocateFrom(ValueLayout.JAVA_INT, lnstyle);
            GraphicNative.trifun.invokeExact(stub, dim, zmin, zstep, zmax, lnSeg, border);
        } catch (Throwable t) {
            throw new RuntimeException("trifun failed", t);
        }
    }

    public static void surmat2(float[][] zmat, int xdim, int ydim) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment ptrArray = arena.allocate(ValueLayout.ADDRESS, ydim);
            for (int i = 0; i < ydim; i++) {
                MemorySegment row = arena.allocateFrom(ValueLayout.JAVA_FLOAT, zmat[i]);
                ptrArray.setAtIndex(ValueLayout.ADDRESS, i, row);
            }
            GraphicNative.surmat2.invokeExact(ptrArray, xdim, ydim);
        } catch (Throwable t) {
            throw new RuntimeException("surmat2 failed", t);
        }
    }

    public static void surmat2(float[][] zmat) {
        int ydim = zmat.length;
        int xdim = ydim > 0 ? zmat[0].length : 0;
        surmat2(zmat, xdim, ydim);
    }

    public static void surfun(BinaryFloatFunction fun, int ixpts, float xdel, int iypts, float ydel) {
        try (Arena arena = Arena.ofConfined()) {
            MethodHandle target = MethodHandles.lookup().findVirtual(
                BinaryFloatFunction.class, "apply",
                MethodType.methodType(float.class, float.class, float.class)
            ).bindTo(fun);
            MemorySegment stub = Linker.nativeLinker().upcallStub(
                target,
                FunctionDescriptor.of(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT),
                arena
            );
            GraphicNative.surfun.invokeExact(stub, ixpts, xdel, iypts, ydel);
        } catch (Throwable t) {
            throw new RuntimeException("surfun failed", t);
        }
    }

    public static void spline(float[] x, float[] y, int n1, float s1, float s2, float[] xn, float[] yn) {
        try (Arena arena = Arena.ofConfined()) {
            int n = Math.min(x.length, y.length);
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            MemorySegment xnSeg = arena.allocate(ValueLayout.JAVA_FLOAT, n1);
            MemorySegment ynSeg = arena.allocate(ValueLayout.JAVA_FLOAT, n1);
            GraphicNative.spline.invokeExact(xSeg, ySeg, n, n1, s1, s2, xnSeg, ynSeg);
            MemorySegment.copy(xnSeg, ValueLayout.JAVA_FLOAT, 0, xn, 0, n1);
            MemorySegment.copy(ynSeg, ValueLayout.JAVA_FLOAT, 0, yn, 0, n1);
        } catch (Throwable t) {
            throw new RuntimeException("spline failed", t);
        }
    }

    public static float[][] spline(float[] x, float[] y, int n1, float s1, float s2) {
        float[] xn = new float[n1];
        float[] yn = new float[n1];
        spline(x, y, n1, s1, s2, xn, yn);
        return new float[][] { xn, yn };
    }

    public static int rgb(int r, int g, int b) {
        try {
            return (int) GraphicNative.gpc_rgb.invokeExact(r, g, b);
        } catch (Throwable t) {
            throw new RuntimeException("rgb failed", t);
        }
    }

    public static int colormapEval(int colormapId, float t) {
        try {
            return (int) GraphicNative.gpc_colormap_eval.invokeExact(colormapId, t);
        } catch (Throwable ex) {
            throw new RuntimeException("colormapEval failed", ex);
        }
    }

    public static int getColor(int colormapId, float value, float minv, float maxv) {
        try {
            return (int) GraphicNative.gpc_get_color.invokeExact(colormapId, value, minv, maxv);
        } catch (Throwable t) {
            throw new RuntimeException("getColor failed", t);
        }
    }

    public static int[] getRGB(int colormapId, float value, float minv, float maxv) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment r = arena.allocate(ValueLayout.JAVA_INT);
            MemorySegment g = arena.allocate(ValueLayout.JAVA_INT);
            MemorySegment b = arena.allocate(ValueLayout.JAVA_INT);
            GraphicNative.gpc_get_rgb.invokeExact(colormapId, value, minv, maxv, r, g, b);
            return new int[] { r.get(ValueLayout.JAVA_INT, 0), g.get(ValueLayout.JAVA_INT, 0), b.get(ValueLayout.JAVA_INT, 0) };
        } catch (Throwable t) {
            throw new RuntimeException("getRGB failed", t);
        }
    }

    public static void scatter(float[] x, float[] y, float[] z, float minv, float maxv, int symnum, float symheight, int colormapId) {
        try (Arena arena = Arena.ofConfined()) {
            int dim = Math.min(x.length, Math.min(y.length, z.length));
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            MemorySegment zSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, z);
            GraphicNative.gpc_scatter.invokeExact(xSeg, ySeg, zSeg, dim, minv, maxv, symnum, symheight, colormapId);
        } catch (Throwable t) {
            throw new RuntimeException("scatter failed", t);
        }
    }

    public static void colorkeyGradient(float minv, float maxv, int colormapId, float legllx, float leglly, float legurx, float legury, int nticks, String format, char fontc, float height, int angle) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment fmtSeg = arena.allocateFrom(format);
            GraphicNative.ColorKeyGradient.invokeExact(minv, maxv, colormapId, legllx, leglly, legurx, legury, nticks, fmtSeg, (byte) fontc, height, angle);
        } catch (Throwable t) {
            throw new RuntimeException("colorkeyGradient failed", t);
        }
    }

    public static void font(String fontname, byte esc) {
        try (Arena arena = Arena.ofConfined()) {
            MemorySegment nameSeg = arena.allocateFrom(fontname);
            GraphicNative.gpc_load_font.invokeExact(nameSeg, esc);
        } catch (Throwable t) {
            throw new RuntimeException("font failed", t);
        }
    }

}
