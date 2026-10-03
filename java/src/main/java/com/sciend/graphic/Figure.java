package com.sciend.graphic;

import java.io.IOException;
import java.lang.foreign.Arena;
import java.lang.foreign.MemorySegment;
import java.lang.foreign.ValueLayout;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;

/**
 * Publication-Quality Scientific Graphics Figure powered by GraphiC.
 * Implements AutoCloseable for idiomatic try-with-resources usage.
 */
public class Figure implements AutoCloseable {

    private final float width;
    private final float height;
    private final Color bgColor;
    private final String tkfName;
    private final Arena arena;

    private boolean active = false;
    private boolean hasAxes = false;
    private String svgContent = null;
    private Path generatedSvgPath = null;

    public Figure(float width, float height, Color bgColor, String tkfName) {
        this.width = width;
        this.height = height;
        this.bgColor = (bgColor != null) ? bgColor : Color.WHITE;
        this.tkfName = (tkfName != null) ? tkfName : "graphic_plot.tkf";
        this.arena = Arena.ofConfined();

        startPlot();
    }

    public Figure(float width, float height) {
        this(width, height, Color.WHITE, "graphic_plot.tkf");
    }

    public Figure() {
        this(8.0f, 6.0f, Color.WHITE, "graphic_plot.tkf");
    }

    private void startPlot() {
        try {
            MemorySegment tkfSeg = arena.allocateFrom(tkfName);
            GraphicNative.bgnplot.invokeExact(1, (int)'g', tkfSeg);
            GraphicNative.metricunits.invokeExact(0);
            GraphicNative.startplot.invokeExact(bgColor.getCode());
            GraphicNative.page.invokeExact(width, height);
            GraphicNative.area2d.invokeExact(width * 0.82f, height * 0.80f);
            active = true;
        } catch (Throwable t) {
            throw new RuntimeException("Failed to initialize GraphiC plot session", t);
        }
    }

    public void box(Color color) {
        try {
            GraphicNative.color.invokeExact(color.getCode());
            GraphicNative.box.invokeExact();
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void box() {
        box(Color.BLACK);
    }

    // Authentic GraphiC routine names
    public void heading(String title) {
        this.title(title);
    }

    public void xname(String name) {
        this.xlabel(name);
    }

    public void yname(String name) {
        this.ylabel(name);
    }

    public void zname(String name) {
        this.zlabel(name);
    }

    public void graf(float xmin, float xstep, float xmax,
                     float ymin, float ystep, float ymax,
                     String xFormat, String yFormat, Color axisColor) {
        setupAxes(xmin, xstep, xmax, ymin, ystep, ymax, xFormat, yFormat, axisColor);
    }

    public void graf(float xmin, float xstep, float xmax,
                     float ymin, float ystep, float ymax) {
        setupAxes(xmin, xstep, xmax, ymin, ystep, ymax);
    }

    public void curve(float[] x, float[] y, Color color, int symbol) {
        this.plot(x, y, color, symbol);
    }

    public void curve(float[] x, float[] y, Color color) {
        this.plot(x, y, color);
    }

    public void curve(float[] x, float[] y) {
        this.plot(x, y, Color.BLACK);
    }

    public void color(Color color) {
        try {
            GraphicNative.color.invokeExact(color.getCode());
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void dashf(LineStyle style) {
        try {
            GraphicNative.dashf.invokeExact(style.getCode());
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void area2d(float xlen, float ylen) {
        try {
            GraphicNative.area2d.invokeExact(xlen, ylen);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void title(String title) {
        try {
            MemorySegment seg = arena.allocateFrom(title);
            GraphicNative.heading.invokeExact(seg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void xlabel(String name) {
        try {
            MemorySegment seg = arena.allocateFrom(name);
            GraphicNative.xname.invokeExact(seg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void ylabel(String name) {
        try {
            MemorySegment seg = arena.allocateFrom(name);
            GraphicNative.yname.invokeExact(seg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void zlabel(String name) {
        try {
            MemorySegment seg = arena.allocateFrom(name);
            GraphicNative.z3name.invokeExact(seg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void setNameSpace(int axis, float factor) {
        try {
            GraphicNative.SetNameSpace.invokeExact(axis, factor);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void setLabelSpace(int axis, float factor) {
        try {
            GraphicNative.SetLabelSpace.invokeExact(axis, factor);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void setupAxes(float xmin, float xstep, float xmax,
                          float ymin, float ystep, float ymax,
                          String xFormat, String yFormat, Color axisColor) {
        try {
            MemorySegment xf = arena.allocateFrom(xFormat);
            MemorySegment yf = arena.allocateFrom(yFormat);
            GraphicNative.color.invokeExact(axisColor.getCode());
            GraphicNative.graf.invokeExact(xf, xmin, xstep, xmax, yf, ymin, ystep, ymax);
            hasAxes = true;
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void setupAxes(float xmin, float xstep, float xmax,
                          float ymin, float ystep, float ymax) {
        setupAxes(xmin, xstep, xmax, ymin, ystep, ymax, "%g", "%g", Color.BLACK);
    }

    public void autoscale(float[] x, float[] y, int nxdiv, int nydiv, Color color) {
        try {
            int n = Math.min(x.length, y.length);
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            GraphicNative.color.invokeExact(color.getCode());
            GraphicNative.scales.invokeExact(nxdiv, nydiv, xSeg, ySeg, n);
            hasAxes = true;
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void autoscale(float[] x, float[] y) {
        autoscale(x, y, 5, 5, Color.BLACK);
    }

    public void plot(float[] x, float[] y, Color color, LineStyle style) {
        try {
            if (!hasAxes) {
                autoscale(x, y);
            }
            int n = Math.min(x.length, y.length);
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);

            GraphicNative.color.invokeExact(color.getCode());
            GraphicNative.dashf.invokeExact(style.getCode());
            GraphicNative.curve.invokeExact(xSeg, ySeg, n, 0);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void plot(float[] x, float[] y, Color color) {
        plot(x, y, color, LineStyle.SOLID);
    }

    public void plot(float[] x, float[] y, Color color, int styleCode) {
        plot(x, y, color, LineStyle.SOLID);
    }

    public void scatter(float[] x, float[] y, Color color, Symbol symbol, int interval) {
        try {
            if (!hasAxes) {
                autoscale(x, y);
            }
            int n = Math.min(x.length, y.length);
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);

            GraphicNative.color.invokeExact(color.getCode());
            GraphicNative.sympick.invokeExact(symbol.getCode());
            GraphicNative.curve.invokeExact(xSeg, ySeg, n, -Math.max(1, interval));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void scatter(float[] x, float[] y, Color color, int symbolCode, int interval) {
        try {
            if (!hasAxes) {
                autoscale(x, y);
            }
            int n = Math.min(x.length, y.length);
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);

            GraphicNative.color.invokeExact(color.getCode());
            GraphicNative.sympick.invokeExact(symbolCode);
            GraphicNative.curve.invokeExact(xSeg, ySeg, n, -Math.max(1, interval));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void scatter(float[] x, float[] y, Color color, Symbol symbol) {
        scatter(x, y, color, symbol, 1);
    }

    public void surface3D(float[] zMatrix, int xdim, int ydim,
                          float[] xlim, float[] ylim, float[] zlim,
                          float[] view, Color topColor, Color botColor, String title) {
        plotSurface(zMatrix, xdim, ydim, xlim, ylim, zlim, topColor, botColor, title);
    }

    public void plotSurface(float[] zMatrix, int xdim, int ydim,
                            float[] xlim, float[] ylim, float[] zlim,
                            Color topColor, Color botColor, String title) {
        try {
            GraphicNative.color.invokeExact(Color.BLACK.getCode());
            GraphicNative.box.invokeExact();
            GraphicNative.tickout.invokeExact(1);

            GraphicNative.volm3d.invokeExact(2.0f, 1.0f, 1.0f);
            GraphicNative.vuabs.invokeExact(8.8f, 3.66f, 3.0f);

            GraphicNative.graf3d.invokeExact(
                    xlim[0], xlim[1], xlim[2],
                    ylim[0], ylim[1], ylim[2],
                    zlim[0], zlim[1], zlim[2]
            );

            GraphicNative.tcolor.invokeExact(topColor.getCode());
            GraphicNative.bcolor.invokeExact(botColor.getCode());
            GraphicNative.d3base.invokeExact(1, 0);

            MemorySegment survisSeg = arena.allocateFrom("both");
            GraphicNative.survis.invokeExact(survisSeg);

            MemorySegment zSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, zMatrix);
            GraphicNative.surmat.invokeExact(zSeg, xdim, ydim);

            if (title != null) {
                MemorySegment titleSeg = arena.allocateFrom(title);
                GraphicNative.color.invokeExact(topColor.getCode());
                GraphicNative.d3head.invokeExact(titleSeg, (int)'t');
            }
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void contour(float[] zMatrix, float[] xCoords, float[] yCoords,
                        float zmin, float zstep, float zmax, Color color, int labelInterval) {
        try {
            int nx = xCoords.length;
            int ny = yCoords.length;
            MemorySegment zSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, zMatrix);
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, xCoords);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, yCoords);

            GraphicNative.color.invokeExact(color.getCode());
            GraphicNative.conmat.invokeExact(zSeg, xSeg, nx, ySeg, ny, zmin, zstep, zmax, MemorySegment.NULL, labelInterval);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void smithChart(Color gridColor, float[][] traces) {
        try {
            GraphicNative.smdraw.invokeExact(gridColor.getCode());
            if (traces != null) {
                GraphicNative.color.invokeExact(Color.WHITE.getCode());
                for (float[] pt : traces) {
                    if (pt.length >= 2) {
                        GraphicNative.plotrx.invokeExact(pt[0], pt[1]);
                    }
                }
            }
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void panelinch(float[] x, float[] y, int pattern, int border) {
        try {
            int n = Math.min(x.length, y.length);
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            GraphicNative.panelinch.invokeExact(xSeg, ySeg, n, pattern, border);
            GraphicNative.panproc.invokeExact();
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void bar(float[] x, float[] y, int nxdiv, float bwid, int mode, Color[] colors, int[] patterns) {
        try {
            int n = Math.min(x.length, y.length);
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);

            MemorySegment cSeg = MemorySegment.NULL;
            if (colors != null && colors.length > 0) {
                int[] cArr = new int[colors.length];
                for (int i = 0; i < colors.length; i++) cArr[i] = colors[i].getCode();
                cSeg = arena.allocateFrom(ValueLayout.JAVA_INT, cArr);
            }

            MemorySegment pSeg = MemorySegment.NULL;
            if (patterns != null && patterns.length > 0) {
                pSeg = arena.allocateFrom(ValueLayout.JAVA_INT, patterns);
            }

            GraphicNative.bar.invokeExact(nxdiv, xSeg, ySeg, n, bwid, mode, cSeg, pSeg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void barchart(int nbars, float[] xarray, float[] yarray, int npts, float width, Color[] carray, int[] typearray) {
        bar(xarray, yarray, nbars, width, 0, carray, typearray);
    }

    public void pie(float[] values, String[] captions, Color[] colors, float[] offsets, float size) {
        try {
            int dim = values.length;
            MemorySegment valSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, values);

            int[] clrArr = new int[dim];
            if (colors != null) {
                for (int i = 0; i < dim; i++) clrArr[i] = -Math.abs(colors[i % colors.length].getCode());
            } else {
                int[] defs = {-4, -1, -2, -5, -3};
                for (int i = 0; i < dim; i++) clrArr[i] = defs[i % defs.length];
            }
            MemorySegment clrSeg = arena.allocateFrom(ValueLayout.JAVA_INT, clrArr);

            float[] offArr = (offsets != null) ? offsets : new float[dim];
            MemorySegment offSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, offArr);

            MemorySegment[] capSegs = new MemorySegment[dim];
            for (int i = 0; i < dim; i++) {
                capSegs[i] = (captions != null && i < captions.length) ? arena.allocateFrom(captions[i]) : arena.allocateFrom("");
            }
            MemorySegment capArraySeg = arena.allocate(ValueLayout.ADDRESS, dim);
            for (int i = 0; i < dim; i++) {
                capArraySeg.setAtIndex(ValueLayout.ADDRESS, i, capSegs[i]);
            }

            GraphicNative.pieplot.invokeExact(dim, valSeg, clrSeg, offSeg, capArraySeg, size, 14, 0);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void polgrid(float rmax, float rmin, int mode, String[] labels, int nray, byte col) {
        try {
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

    public void polcurve(float[] r, float[] theta, int mark) {
        try {
            int n = Math.min(r.length, theta.length);
            MemorySegment rSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, r);
            MemorySegment thSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, theta);
            GraphicNative.polcurve.invokeExact(rSeg, thSeg, n, mark);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void xlog(float xmin, float xmax, String form, float yorig, float ystep, float ymax) {
        try {
            MemorySegment formSeg = arena.allocateFrom(form);
            GraphicNative.xlog.invokeExact(xmin, xmax, formSeg, yorig, ystep, ymax);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void lglgscle(float[] x, float[] y) {
        try {
            int n = Math.min(x.length, y.length);
            MemorySegment xSeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, x);
            MemorySegment ySeg = arena.allocateFrom(ValueLayout.JAVA_FLOAT, y);
            GraphicNative.lglgscle.invokeExact(xSeg, ySeg, n);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void vector(float x1, float y1, float x2, float y2, float headAngle, float headSize, String opt) {
        try {
            MemorySegment optSeg = arena.allocateFrom(opt);
            GraphicNative.uvector.invokeExact(x1, y1, x2, y2, headAngle, headSize, optSeg);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void d3bars(float bwid) {
        try {
            GraphicNative.d3bars.invokeExact(bwid);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void plane3d(char wplane, float pos, char hide) {
        Graphic.plane3d(wplane, pos, hide);
    }

    public void waterfallPlot(float[] x, float[] y, float[][] z) {
        Graphic.waterfallPlot(x, y, z);
    }

    public void WaterfallPlot(float[] x, float[] y, float[][] z) {
        Graphic.waterfallPlot(x, y, z);
    }

    public void waterfallCurve(float[] x, float y, float[] z) {
        Graphic.waterfallCurve(x, y, z);
    }

    public void WaterfallCurve(float[] x, float y, float[] z) {
        Graphic.waterfallCurve(x, y, z);
    }

    public void ternaryNames(String aName, String bName, String cName) {
        try {
            GraphicNative.aname.invokeExact(arena.allocateFrom(aName));
            GraphicNative.bname.invokeExact(arena.allocateFrom(bName));
            GraphicNative.cname.invokeExact(arena.allocateFrom(cName));
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void simuplot(int nplots) {
        try {
            GraphicNative.simuplot.invokeExact(nplots);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void context(int icontext) {
        try {
            GraphicNative.context.invokeExact(icontext);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void orel(float dx, float dy) {
        try {
            GraphicNative.orel.invokeExact(dx, dy);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void physor(float x, float y) {
        try {
            GraphicNative.physor.invokeExact(x, y);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void axesoff(int bits) {
        try {
            GraphicNative.axesoff.invokeExact(bits);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public void pltfnt(float x, float y, String text, float size, int angle) {
        try {
            MemorySegment textSeg = arena.allocateFrom(text);
            GraphicNative.pltfnt.invokeExact(x, y, textSeg, size, angle);
        } catch (Throwable t) {
            throw new RuntimeException(t);
        }
    }

    public String render() {
        if (active) {
            try {
                int endCode = (int) GraphicNative.endplot.invokeExact();
                GraphicNative.stopplot.invokeExact();
                active = false;

                MemorySegment pathSeg = (MemorySegment) GraphicNative.gpc_get_svg_path.invokeExact();
                String pathStr = pathSeg.reinterpret(1024).getString(0, StandardCharsets.UTF_8);
                generatedSvgPath = Path.of(pathStr);

                if (Files.exists(generatedSvgPath)) {
                    svgContent = Files.readString(generatedSvgPath);
                }
            } catch (Throwable t) {
                throw new RuntimeException("Error finalizing GraphiC plot", t);
            }
        }
        return svgContent;
    }

    public void box3d() {
        Graphic.box3d();
    }

    public void cross(int iscross) {
        Graphic.cross(iscross);
    }

    public void CrossAt(float x, float y) {
        Graphic.CrossAt(x, y);
    }

    public void frame(int frameon, int ftick) {
        Graphic.frame(frameon, ftick);
    }

    public void tickht(float height) {
        Graphic.tickht(height);
    }

    public void tickout(int is_out) {
        Graphic.tickout(is_out);
    }

    public void numht(float height) {
        Graphic.numht(height);
    }

    public void titlht(float height) {
        Graphic.titlht(height);
    }

    public void axnamht(float height) {
        Graphic.axnamht(height);
    }

    public void upright(int uplab) {
        Graphic.upright(uplab);
    }

    public void acrop(int flag) {
        Graphic.acrop(flag);
    }

    public void svgcrop(float marginInches) {
        Graphic.svgcrop(marginInches);
    }

    public void svgCrop(float marginInches) {
        Graphic.svgcrop(marginInches);
    }

    public int svgopen(String filepath) {
        return Graphic.svgopen(filepath);
    }

    public int svgopen() {
        return Graphic.svgopen();
    }

    public int svg_open(String filepath) {
        return Graphic.svgopen(filepath);
    }

    public int svg_open() {
        return Graphic.svgopen();
    }

    public int svgpush(String url, String filepath) {
        return Graphic.svgpush(url, filepath);
    }

    public int svgpush(String url) {
        return Graphic.svgpush(url);
    }

    public int svg_push(String url, String filepath) {
        return Graphic.svgpush(url, filepath);
    }

    public int svg_push(String url) {
        return Graphic.svgpush(url);
    }

    public void rotate(int isrot) {
        Graphic.rotate(isrot);
    }

    public void pgshift(float x, float y) {
        Graphic.pgshift(x, y);
    }

    public void fgrid(int fon, int ndiv) {
        Graphic.fgrid(fon, ndiv);
    }

    public void xfgrid(int fon, int nx) {
        Graphic.xfgrid(fon, nx);
    }

    public void yfgrid(int fon, int ny) {
        Graphic.yfgrid(fon, ny);
    }

    public void d3line(float x1, float y1, float z1, float x2, float y2, float z2) {
        Graphic.d3line(x1, y1, z1, x2, y2, z2);
    }

    public void d3color(int[] colors, int nlevels, int mesh) {
        Graphic.d3color(colors, nlevels, mesh);
    }

    public void sclfix(int isfixed) {
        Graphic.sclfix(isfixed);
    }

    public void nohide(int visible) {
        Graphic.nohide(visible);
    }

    public void vuangl(float phi, float theta, float radius) {
        Graphic.vuangl(phi, theta, radius);
    }

    public void contcolor(int[] colors, int border) {
        Graphic.contcolor(colors, border);
    }

    public void ctline(String text, float height) {
        Graphic.ctline(text, height);
    }

    public void ltline(String text, float height) {
        Graphic.ltline(text, height);
    }

    public void rtline(String text, float height) {
        Graphic.rtline(text, height);
    }

    public float strwidth(String text, float height) {
        return Graphic.strwidth(text, height);
    }

    public void subsym(char ch) {
        Graphic.subsym(ch);
    }

    public void legend(int marker, String text, int style, float height, int color) {
        Graphic.legend(marker, text, style, height, color);
    }

    public void legpos(int entries, float x, float y, int border) {
        Graphic.legpos(entries, x, y, border);
    }

    public void loglog(float xorig, float xmax, float yorig, float ymax) {
        Graphic.loglog(xorig, xmax, yorig, ymax);
    }

    public void tline(int width) {
        Graphic.tline(width);
    }

    public void tcurve(int width) {
        Graphic.tcurve(width);
    }

    public void plotadd() {
        Graphic.plotadd();
    }

    public void dateit(char font) {
        Graphic.dateit(font);
    }

    // Hershey font state registers
    public void linesp(float ratio) {
        Graphic.linesp(ratio);
    }

    public void lmargin(float marginch) {
        Graphic.lmargin(marginch);
    }

    public void rmargin(float marginch) {
        Graphic.rmargin(marginch);
    }

    public void tmargin(float marginch) {
        Graphic.tmargin(marginch);
    }

    public void charspc(int width) {
        Graphic.charspc(width);
    }

    public void skew(float slant) {
        Graphic.skew(slant);
    }

    public void widen(float fact) {
        Graphic.widen(fact);
    }

    public void tfont(int wid) {
        Graphic.tfont(wid);
    }

    public void tifont(float wid) {
        Graphic.tifont(wid);
    }

    public void fntchg(char sym) {
        Graphic.fntchg(sym);
    }

    public void supsym(char ch) {
        Graphic.supsym(ch);
    }

    public void putlabel(String text, float xpos, float ypos, float height, int border, int units) {
        Graphic.putlabel(text, xpos, ypos, height, border, units);
    }

    public void ylog(float xorig, float xstep, float xmax, float yorig, float ymax, String format) {
        Graphic.ylog(format, xorig, xstep, xmax, yorig, ymax);
    }

    public void prtfnt(float xinch, float yinch, String text, float height, int angle) {
        Graphic.prtfnt(xinch, yinch, text, height, angle);
    }

    public void x3name(String text) {
        Graphic.x3name(text);
    }

    public void y3name(String text) {
        Graphic.y3name(text);
    }

    public void z3name(String text) {
        Graphic.z3name(text);
    }

    public void d3head(String text, int font) {
        Graphic.d3head(text, font);
    }

    public void tcolor(int val) {
        Graphic.tcolor(val);
    }

    public void bcolor(int val) {
        Graphic.bcolor(val);
    }

    public void volm3d(float xlen, float ylen, float zlen) {
        Graphic.volm3d(xlen, ylen, zlen);
    }

    public void vuabs(float r, float theta, float phi) {
        Graphic.vuabs(r, theta, phi);
    }

    public void graf3d(float xmin, float xstep, float xmax, float ymin, float ystep, float ymax, float zmin, float zstep, float zmax) {
        Graphic.graf3d(xmin, xstep, xmax, ymin, ystep, ymax, zmin, zstep, zmax);
    }

    public void survis(float[] angles) {
        Graphic.survis(angles);
    }

    public void d3base(int isbase, int colorb) {
        Graphic.d3base(isbase, colorb);
    }

    public void surmat(float[] z, int nx, int ny) {
        Graphic.surmat(z, nx, ny);
    }

    public void conmat(float[] z, float[] x, int nx, float[] y, int ny, float zmin, float zstep, float zmax, int labelInterval) {
        Graphic.conmat(z, x, nx, y, ny, zmin, zstep, zmax, labelInterval);
    }

    public void curv3d(float[] x, float[] y, float[] z, int n) {
        Graphic.curv3d(x, y, z, n);
    }

    public void curvfill(float[] x1, float[] y1, int n1, float[] x2, float[] y2, int n2, int color, int pattern) {
        Graphic.curvfill(x1, y1, n1, x2, y2, n2, color, pattern);
    }

    public void errorbar(float x, float y, float dx1, float dx2, float dy1, float dy2) {
        Graphic.errorbar(x, y, dx1, dx2, dy1, dy2);
    }

    public void scatplot(char type) {
        Graphic.scatplot(type);
    }

    public void histogram(float[] x, float[] y, int n, int fill, int[] colors) {
        Graphic.histogram(x, y, n, fill, colors);
    }

    public void wedge(float rmin, float rmax, float thmin, float thmax, int color, int pattern) {
        Graphic.wedge(rmin, rmax, thmin, thmax, color, pattern);
    }

    public void stack(char mode) {
        Graphic.stack(mode);
    }

    public void plotrho(float xlen, float ylen) {
        Graphic.plotrho(xlen, ylen);
    }

    public void smithcir(float r, float x, float rad) {
        Graphic.smithcir(r, x, rad);
    }

    public void smove(float angle) {
        Graphic.smove(angle);
    }

    public void plotabsolute(int flag) {
        Graphic.plotabsolute(flag);
    }

    public void newFile(String filename) {
        Graphic.newFile(filename);
    }

    public void NewFile(String filename) {
        Graphic.NewFile(filename);
    }

    public void aname(String text) {
        Graphic.aname(text);
    }

    public void bname(String text) {
        Graphic.bname(text);
    }

    public void cname(String text) {
        Graphic.cname(text);
    }

    public void metricunits(int flag) {
        Graphic.metricunits(flag);
    }

    public void scales(int type, int code, float[] x, float[] y, int n) {
        Graphic.scales(type, code, x, y, n);
    }

    public void smdraw(int smon) {
        Graphic.smdraw(smon);
    }

    public void plotrx(float xlen, float ylen) {
        Graphic.plotrx(xlen, ylen);
    }

    public void panproc() {
        Graphic.panproc();
    }

    public void fillfont(int fill) {
        Graphic.fillfont(fill);
    }

    public void fontfill(int fill, int pattern, int border) {
        Graphic.fontfill(fill, pattern, border);
    }

    public void surmat2(float[][] zmat, int xdim, int ydim) {
        Graphic.surmat2(zmat, xdim, ydim);
    }

    public void surmat2(float[][] zmat) {
        Graphic.surmat2(zmat);
    }

    public void surfun(BinaryFloatFunction fun, int ixpts, float xdel, int iypts, float ydel) {
        Graphic.surfun(fun, ixpts, xdel, iypts, ydel);
    }

    public void trimat(float[] m1, float[] m2, float[] z, int dim1,
                       float zmin, float zstep, float zmax, int[] lnstyle, int border) {
        Graphic.trimat(m1, m2, z, dim1, zmin, zstep, zmax, lnstyle, border);
    }

    public void trifun(BinaryFloatFunction fun, int dim, float zmin, float zstep, float zmax, int[] lnstyle, int border) {
        Graphic.trifun(fun, dim, zmin, zstep, zmax, lnstyle, border);
    }

    public void spline(float[] x, float[] y, int n1, float s1, float s2, float[] xn, float[] yn) {
        Graphic.spline(x, y, n1, s1, s2, xn, yn);
    }

    public float[][] spline(float[] x, float[] y, int n1, float s1, float s2) {
        return Graphic.spline(x, y, n1, s1, s2);
    }

    public float[] usertoinch(float x, float y) {
        return Graphic.usertoinch(x, y);
    }

    public int[] usertopix(float x, float y, float z) {
        return Graphic.usertopix(x, y, z);
    }

    public float stdDev(float[] x) {
        return Graphic.stdDev(x);
    }

    public float stddev(float[] x) {
        return Graphic.stdDev(x);
    }

    public float StdDev(float[] x) {
        return Graphic.stdDev(x);
    }

    public void settolerance(float tolerance) {
        Graphic.settolerance(tolerance);
    }

    public void save(Path outputPath) throws IOException {
        render();
        if (generatedSvgPath != null && Files.exists(generatedSvgPath)) {
            if (outputPath.getParent() != null) {
                Files.createDirectories(outputPath.getParent());
            }
            Files.copy(generatedSvgPath, outputPath, StandardCopyOption.REPLACE_EXISTING);
        }
    }

    public void save(String outputPath) throws IOException {
        save(Path.of(outputPath));
    }

    public void saveHtml(Path outputPath) throws IOException {
        render();
        if (generatedSvgPath != null) {
            String htmlName = generatedSvgPath.getFileName().toString().replace(".svg", ".html");
            Path htmlSrc = generatedSvgPath.resolveSibling(htmlName);
            if (Files.exists(htmlSrc)) {
                if (outputPath.getParent() != null) {
                    Files.createDirectories(outputPath.getParent());
                }
                Files.copy(htmlSrc, outputPath, StandardCopyOption.REPLACE_EXISTING);
            }
        }
    }

    public void saveHtml(String outputPath) throws IOException {
        saveHtml(Path.of(outputPath));
    }

    @Override
    public void close() {
        try {
            if (active) {
                render();
            }
        } finally {
            arena.close();
        }
    }
}
