package com.sciend.graphic;

import java.lang.foreign.*;
import java.lang.invoke.MethodHandle;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Optional;

/**
 * Low-level Foreign Function & Memory (FFM) bindings to libgraphic.
 */
public class GraphicNative {

    private static final Linker LINKER = Linker.nativeLinker();
    private static final SymbolLookup LOOKUP;

    public static final MethodHandle bgnplot;
    public static final MethodHandle startplot;
    public static final MethodHandle endplot;
    public static final MethodHandle stopplot;
    public static final MethodHandle page;
    public static final MethodHandle area2d;
    public static final MethodHandle metricunits;
    public static final MethodHandle box;
    public static final MethodHandle pbox;
    public static final MethodHandle color;
    public static final MethodHandle tcolor;
    public static final MethodHandle bcolor;
    public static final MethodHandle heading;
    public static final MethodHandle xname;
    public static final MethodHandle yname;
    public static final MethodHandle x3name;
    public static final MethodHandle y3name;
    public static final MethodHandle z3name;
    public static final MethodHandle d3head;
    public static final MethodHandle tickout;
    public static final MethodHandle dashf;
    public static final MethodHandle graf;
    public static final MethodHandle scales;
    public static final MethodHandle curve;
    public static final MethodHandle sympick;
    public static final MethodHandle volm3d;
    public static final MethodHandle vuabs;
    public static final MethodHandle graf3d;
    public static final MethodHandle survis;
    public static final MethodHandle d3base;
    public static final MethodHandle surmat;
    public static final MethodHandle conmat;
    public static final MethodHandle smdraw;
    public static final MethodHandle plotrx;
    public static final MethodHandle panelinch;
    public static final MethodHandle panproc;
    public static final MethodHandle bar;
    public static final MethodHandle pieplot;
    public static final MethodHandle polgrid;
    public static final MethodHandle polcurve;
    public static final MethodHandle xlog;
    public static final MethodHandle lglgscle;
    public static final MethodHandle uvector;
    public static final MethodHandle d3bars;
    public static final MethodHandle aname;
    public static final MethodHandle bname;
    public static final MethodHandle cname;
    public static final MethodHandle simuplot;
    public static final MethodHandle context;
    public static final MethodHandle orel;
    public static final MethodHandle physor;
    public static final MethodHandle axesoff;
    public static final MethodHandle pltfnt;
    public static final MethodHandle gpc_get_svg_path;

    // Extended bindings
    public static final MethodHandle box3d;
    public static final MethodHandle cross;
    public static final MethodHandle CrossAt;
    public static final MethodHandle frame;
    public static final MethodHandle tickht;
    public static final MethodHandle numht;
    public static final MethodHandle titlht;
    public static final MethodHandle axnamht;
    public static final MethodHandle upright;
    public static final MethodHandle acrop;
    public static final MethodHandle svgcrop;
    public static final MethodHandle svgopen;
    public static final MethodHandle svgpush;
    public static final MethodHandle rotate;
    public static final MethodHandle pgshift;
    public static final MethodHandle fgrid;
    public static final MethodHandle d3line;
    public static final MethodHandle curv3d;
    public static final MethodHandle d3color;
    public static final MethodHandle nohide;
    public static final MethodHandle vuangl;
    public static final MethodHandle contcolor_fixed;
    public static final MethodHandle curvfill;
    public static final MethodHandle errorbar;
    public static final MethodHandle histogram;
    public static final MethodHandle ctline;
    public static final MethodHandle ltline;
    public static final MethodHandle rtline;
    public static final MethodHandle strwidth;
    public static final MethodHandle subsym;
    public static final MethodHandle legend;
    public static final MethodHandle legpos;
    public static final MethodHandle loglog;
    public static final MethodHandle stack;
    public static final MethodHandle plotrho;
    public static final MethodHandle smithcir;
    public static final MethodHandle smove;
    public static final MethodHandle tline;
    public static final MethodHandle tcurve;
    public static final MethodHandle plotadd;
    public static final MethodHandle plotabsolute;
    public static final MethodHandle NewFile;
    public static final MethodHandle dateit;
    public static final MethodHandle fillfont;
    public static final MethodHandle fontfill;

    // Hershey font state registers
    public static final MethodHandle linesp;
    public static final MethodHandle lmargin;
    public static final MethodHandle rmargin;
    public static final MethodHandle tmargin;
    public static final MethodHandle charspc;
    public static final MethodHandle skew;
    public static final MethodHandle widen;
    public static final MethodHandle tfont;
    public static final MethodHandle tifont;
    public static final MethodHandle fntchg;
    public static final MethodHandle supsym;
    public static final MethodHandle putlabel;
    public static final MethodHandle ylog;
    public static final MethodHandle prtfnt;

    
    public static final MethodHandle BoxPlot;
    public static final MethodHandle WaterfallPlot;
    public static final MethodHandle WaterfallCurve;
    public static final MethodHandle plane3d;
    public static final MethodHandle surmat2;
    public static final MethodHandle surfun;
    public static final MethodHandle trimat;
    public static final MethodHandle trifun;
    public static final MethodHandle lconmat;
    public static final MethodHandle ColorKeyL;
    public static final MethodHandle ColorKeyN;
    public static final MethodHandle erfcurve;
    public static final MethodHandle erfgrid;
    public static final MethodHandle erfscales;
    public static final MethodHandle usertoinch;
    public static final MethodHandle usertopix;
    public static final MethodHandle intopix;
    public static final MethodHandle pixtoin;
    public static final MethodHandle ellipse;
    public static final MethodHandle fillellipse;
    public static final MethodHandle spline;
    public static final MethodHandle blank;
    public static final MethodHandle vector;
    public static final MethodHandle d3grid;
    public static final MethodHandle d3pltfnt;
    public static final MethodHandle HLStoRGB;
    public static final MethodHandle RGBtoHLS;
    public static final MethodHandle RGBtoCMY;
    public static final MethodHandle RGBtoCMYK;
    public static final MethodHandle RGBtoIBM;
    public static final MethodHandle RGBcolor;
    public static final MethodHandle mean;
    public static final MethodHandle median;
    public static final MethodHandle minmax;
    public static final MethodHandle StdDev;
    public static final MethodHandle matsolve;
    public static final MethodHandle matinverse;
    public static final MethodHandle SetTTFull;
    public static final MethodHandle SetTTKerning;
    public static final MethodHandle SetTTMaxError;
    public static final MethodHandle SetTTUseMemory;
    public static final MethodHandle settolerance;
    public static final MethodHandle saveplotq;
    public static final MethodHandle rowcolq;
    public static final MethodHandle curson;
    public static final MethodHandle curvefollow;
    public static final MethodHandle GetDataPoint;
    public static final MethodHandle scatplot;
    public static final MethodHandle sclfix;
    public static final MethodHandle wedge;
    public static final MethodHandle xfgrid;
    
    public static final MethodHandle pixtouser;
    public static final MethodHandle rfit;
    public static final MethodHandle gpcBeep;
    public static final MethodHandle set_time;
    public static final MethodHandle get_time;
    public static final MethodHandle yfgrid;
    public static final MethodHandle SetNameSpace;
    public static final MethodHandle SetLabelSpace;
    public static final MethodHandle hardcopy;

    public static final MethodHandle gpc_rgb;
    public static final MethodHandle gpc_colormap_eval;
    public static final MethodHandle gpc_get_color;
    public static final MethodHandle gpc_get_rgb;
    public static final MethodHandle gpc_scatter;
    public static final MethodHandle ColorKeyGradient;
    public static final MethodHandle gpc_load_font;

    static {
        Path libPath = findLibrary();
        LOOKUP = SymbolLookup.libraryLookup(libPath, Arena.global());

        SetNameSpace = downcall("SetNameSpace", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT));
        SetLabelSpace = downcall("SetLabelSpace", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT));
        hardcopy = downcall("hardcopy", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE));

        bgnplot = downcall("bgnplot", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.ADDRESS));
        startplot = downcall("startplot", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        endplot = downcall("endplot", FunctionDescriptor.of(ValueLayout.JAVA_INT));
        stopplot = downcall("stopplot", FunctionDescriptor.ofVoid());

        page = downcall("page", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT));
        area2d = downcall("area2d", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT));
        metricunits = downcall("metricunits", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));

        box = downcall("box", FunctionDescriptor.ofVoid());
        pbox = downcall("pbox", FunctionDescriptor.ofVoid());

        color = downcall("color", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        tcolor = downcall("tcolor", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        bcolor = downcall("bcolor", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));

        heading = downcall("heading", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS));
        xname = downcall("xname", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS));
        yname = downcall("yname", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS));

        x3name = downcall("x3name", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS));
        y3name = downcall("y3name", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS));
        z3name = downcall("z3name", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS));
        d3head = downcall("d3head", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.JAVA_INT));

        tickout = downcall("tickout", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        dashf = downcall("dashf", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));

        graf = downcall("graf", FunctionDescriptor.ofVoid(
                ValueLayout.ADDRESS, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT,
                ValueLayout.ADDRESS, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT
        ));
        scales = downcall("scales", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_INT, ValueLayout.JAVA_INT,
                ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT
        ));
        curve = downcall("curve", FunctionDescriptor.ofVoid(
                ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT
        ));
        sympick = downcall("sympick", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));

        volm3d = downcall("volm3d", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT));
        vuabs = downcall("vuabs", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT));
        graf3d = downcall("graf3d", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT,
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT,
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT
        ));
        survis = downcall("survis", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS));
        d3base = downcall("d3base", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));
        surmat = downcall("surmat", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));

        conmat = downcall("conmat", FunctionDescriptor.ofVoid(
                ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT,
                ValueLayout.ADDRESS, ValueLayout.JAVA_INT,
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT,
                ValueLayout.ADDRESS, ValueLayout.JAVA_INT
        ));

        smdraw = downcall("smdraw", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        plotrx = downcall("plotrx", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT));

        panelinch = downcall("panelinch", FunctionDescriptor.ofVoid(
                ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT
        ));
        panproc = downcall("panproc", FunctionDescriptor.ofVoid());

        bar = downcall("bar", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT,
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.ADDRESS
        ));

        pieplot = downcall("pieplot", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.ADDRESS,
                ValueLayout.ADDRESS, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT
        ));

        polgrid = downcall("polgrid", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT,
                ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_BYTE
        ));
        polcurve = downcall("polcurve", FunctionDescriptor.ofVoid(
                ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT
        ));

        xlog = downcall("xlog", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS,
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT
        ));
        lglgscle = downcall("lglgscle", FunctionDescriptor.ofVoid(
                ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT
        ));

        uvector = downcall("uvector", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT,
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS
        ));

        d3bars = downcall("d3bars", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));

        aname = downcall("aname", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS));
        bname = downcall("bname", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS));
        cname = downcall("cname", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS));

        simuplot = downcall("simuplot", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        context = downcall("context", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        orel = downcall("orel", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT));
        physor = downcall("physor", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT));
        axesoff = downcall("axesoff", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        pltfnt = downcall("pltfnt", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT
        ));

        gpc_get_svg_path = downcall("gpc_get_svg_path", FunctionDescriptor.of(ValueLayout.ADDRESS));

        box3d = downcall("box3d", FunctionDescriptor.ofVoid());
        cross = downcall("cross", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE));
        CrossAt = downcall("CrossAt", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT));
        frame = downcall("frame", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE, ValueLayout.JAVA_BYTE));
        tickht = downcall("tickht", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));
        numht = downcall("numht", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));
        titlht = downcall("titlht", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));
        axnamht = downcall("axnamht", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));
        upright = downcall("upright", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE));
        acrop = downcall("acrop", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE));
        svgcrop = downcall("svgcrop", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));
        svgopen = downcall("svgopen", FunctionDescriptor.of(ValueLayout.JAVA_INT, ValueLayout.ADDRESS));
        svgpush = downcall("svgpush", FunctionDescriptor.of(ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.ADDRESS));
        rotate = downcall("rotate", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE));
        pgshift = downcall("pgshift", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT));
        fgrid = downcall("fgrid", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));

        d3line = downcall("d3line", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT,
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT
        ));
        curv3d = downcall("curv3d", FunctionDescriptor.ofVoid(
                ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT
        ));
        d3color = downcall("d3color", FunctionDescriptor.ofVoid(
                ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_BYTE
        ));
        nohide = downcall("nohide", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        vuangl = downcall("vuangl", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT
        ));

        contcolor_fixed = downcall("contcolor_fixed", FunctionDescriptor.ofVoid(
                ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.ADDRESS
        ));
        curvfill = downcall("curvfill", FunctionDescriptor.ofVoid(
                ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT,
                ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT,
                ValueLayout.JAVA_INT, ValueLayout.JAVA_INT
        ));
        errorbar = downcall("errorbar", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT,
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT
        ));
        histogram = downcall("histogram", FunctionDescriptor.ofVoid(
                ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT,
                ValueLayout.JAVA_INT, ValueLayout.ADDRESS
        ));

        ctline = downcall("ctline", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.JAVA_FLOAT));
        ltline = downcall("ltline", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.JAVA_FLOAT));
        rtline = downcall("rtline", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.JAVA_FLOAT));
        strwidth = downcall("strwidth", FunctionDescriptor.of(ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.JAVA_FLOAT));
        subsym = downcall("subsym", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE));
        legend = downcall("legend", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.JAVA_INT,
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT
        ));
        legpos = downcall("legpos", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT
        ));

        loglog = downcall("loglog", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT
        ));
        wedge = downcall("wedge", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT,
                ValueLayout.JAVA_INT, ValueLayout.JAVA_INT
        ));
        stack = downcall("stack", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE));
        plotrho = downcall("plotrho", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT));
        smithcir = downcall("smithcir", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT));
        smove = downcall("smove", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));
        tline = downcall("tline", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        tcurve = downcall("tcurve", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        plotadd = downcall("plotadd", FunctionDescriptor.ofVoid());
        plotabsolute = downcall("plotabsolute", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        NewFile = downcall("NewFile", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS));
        dateit = downcall("dateit", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE));
        fillfont = downcall("fillfont", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        fontfill = downcall("fontfill", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));

        // Hershey font state registers
        linesp = downcall("linesp", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));
        lmargin = downcall("lmargin", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));
        rmargin = downcall("rmargin", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));
        tmargin = downcall("tmargin", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));
        charspc = downcall("charspc", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        skew = downcall("skew", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));
        widen = downcall("widen", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));
        tfont = downcall("tfont", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        tifont = downcall("tifont", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));
        fntchg = downcall("fntchg", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE));
        supsym = downcall("supsym", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE));
        putlabel = downcall("putlabel", FunctionDescriptor.ofVoid(
                ValueLayout.ADDRESS, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT,
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT
        ));
        ylog = downcall("ylog", FunctionDescriptor.ofVoid(
                ValueLayout.ADDRESS, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT,
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT
        ));
        prtfnt = downcall("prtfnt", FunctionDescriptor.ofVoid(
                ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT
        ));
    
        // Full Parity Downcall Inits
        BoxPlot = downcall("BoxPlot", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT));
        WaterfallPlot = downcall("WaterfallPlot", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));
        WaterfallCurve = downcall("WaterfallCurve", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.JAVA_INT));
        plane3d = downcall("plane3d", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_BYTE));
        surmat2 = downcall("surmat2", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));
        surfun = downcall("surfun", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT));
        trimat = downcall("trimat", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.JAVA_INT));
        trifun = downcall("trifun", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.JAVA_INT));
        lconmat = downcall("lconmat", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.ADDRESS));
        ColorKeyL = downcall("ColorKeyL", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.JAVA_BYTE, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT));
        ColorKeyN = downcall("ColorKeyN", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.JAVA_BYTE, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT));
        erfcurve = downcall("erfcurve", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));
        erfgrid = downcall("erfgrid", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS));
        erfscales = downcall("erfscales", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.JAVA_INT));
        usertoinch = downcall("usertoinch", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.ADDRESS));
        usertopix = downcall("usertopix", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.ADDRESS));
        intopix = downcall("intopix", FunctionDescriptor.of(ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT));
        pixtoin = downcall("pixtoin", FunctionDescriptor.of(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT));
        ellipse = downcall("ellipse", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_BYTE, ValueLayout.JAVA_INT));
        fillellipse = downcall("fillellipse", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));
        spline = downcall("spline", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.ADDRESS));
        blank = downcall("blank", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        vector = downcall("vector", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT));
        d3grid = downcall("d3grid", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));
        d3pltfnt = downcall("d3pltfnt", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT));
        HLStoRGB = downcall("HLStoRGB", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.ADDRESS));
        RGBtoHLS = downcall("RGBtoHLS", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.ADDRESS));
        RGBtoCMY = downcall("RGBtoCMY", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.ADDRESS));
        RGBtoCMYK = downcall("RGBtoCMYK", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.ADDRESS));
        RGBtoIBM = downcall("RGBtoIBM", FunctionDescriptor.of(ValueLayout.JAVA_INT, ValueLayout.ADDRESS));
        RGBcolor = downcall("RGBcolor", FunctionDescriptor.of(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));
        mean = downcall("mean", FunctionDescriptor.of(ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.JAVA_INT));
        median = downcall("median", FunctionDescriptor.of(ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.JAVA_INT));
        minmax = downcall("minmax", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT));
        StdDev = downcall("StdDev", FunctionDescriptor.of(ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.JAVA_INT));
        matsolve = downcall("matsolve", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.ADDRESS));
        matinverse = downcall("matinverse", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT));
        SetTTFull = downcall("SetTTFull", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE, ValueLayout.JAVA_BYTE));
        SetTTKerning = downcall("SetTTKerning", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE, ValueLayout.JAVA_BYTE));
        SetTTMaxError = downcall("SetTTMaxError", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_BYTE));
        SetTTUseMemory = downcall("SetTTUseMemory", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE));
        settolerance = downcall("settolerance", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT));
        saveplotq = downcall("saveplotq", FunctionDescriptor.ofVoid());
        rowcolq = downcall("rowcolq", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));
        curson = downcall("curson", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.ADDRESS));
        curvefollow = downcall("curvefollow", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT));
        GetDataPoint = downcall("GetDataPoint", FunctionDescriptor.of(ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.ADDRESS));
        scatplot = downcall("scatplot", FunctionDescriptor.ofVoid(ValueLayout.JAVA_BYTE));
        sclfix = downcall("sclfix", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        
        xfgrid = downcall("xfgrid", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));
        yfgrid = downcall("yfgrid", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));
        pixtouser = downcall("pixtouser", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.ADDRESS));
        rfit = downcall("rfit", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.ADDRESS));
        gpcBeep = downcall("gpcBeep", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));
        set_time = downcall("set_time", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT));
        get_time = downcall("get_time", FunctionDescriptor.of(ValueLayout.ADDRESS, ValueLayout.JAVA_INT));

        gpc_rgb = downcall("gpc_rgb", FunctionDescriptor.of(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_INT));
        gpc_colormap_eval = downcall("gpc_colormap_eval", FunctionDescriptor.of(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT));
        gpc_get_color = downcall("gpc_get_color", FunctionDescriptor.of(ValueLayout.JAVA_INT, ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT));
        gpc_get_rgb = downcall("gpc_get_rgb", FunctionDescriptor.ofVoid(ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.ADDRESS));
        gpc_scatter = downcall("gpc_scatter", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.ADDRESS, ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT));
        ColorKeyGradient = downcall("ColorKeyGradient", FunctionDescriptor.ofVoid(ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT, ValueLayout.ADDRESS, ValueLayout.JAVA_BYTE, ValueLayout.JAVA_FLOAT, ValueLayout.JAVA_INT));
        gpc_load_font = downcall("gpc_load_font", FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.JAVA_BYTE));

    }

    private static MethodHandle downcall(String name, FunctionDescriptor descriptor) {
        Optional<MemorySegment> symbol = LOOKUP.find(name);
        if (symbol.isEmpty()) {
            throw new UnsatisfiedLinkError("Symbol not found in GraphiC library: " + name);
        }
        return LINKER.downcallHandle(symbol.get(), descriptor);
    }

    private static Path findLibrary() {
        String env = System.getenv("GRAPHIC_LIB_PATH");
        if (env != null && Files.exists(Path.of(env))) {
            return Path.of(env);
        }

        String os = System.getProperty("os.name").toLowerCase();
        String[] libNames;
        if (os.contains("win")) {
            libNames = new String[] { "graphic.dll", "libgraphic.dll" };
        } else if (os.contains("mac")) {
            libNames = new String[] { "libgraphic.dylib", "libgraphic.so" };
        } else {
            libNames = new String[] { "libgraphic.so", "libgraphic.dylib" };
        }

        String[] searchDirs = new String[] {
            ".",
            "lib",
            "../lib",
            "../../lib",
            "bin",
            "../bin",
            "../../bin",
            "build",
            "../build",
            "../../build",
            System.getProperty("user.dir"),
            Path.of(System.getProperty("user.dir"), "lib").toString(),
            Path.of(System.getProperty("user.dir"), "build").toString(),
            Path.of(System.getProperty("user.dir"), "bin").toString(),
            "/usr/local/lib",
            "/opt/homebrew/lib"
        };

        for (String dir : searchDirs) {
            for (String name : libNames) {
                Path p = Path.of(dir, name);
                if (Files.exists(p)) {
                    return p.toAbsolutePath();
                }
            }
        }

        throw new IllegalStateException("Could not find GraphiC shared library. Please set GRAPHIC_LIB_PATH or compile it.");
    }
}
