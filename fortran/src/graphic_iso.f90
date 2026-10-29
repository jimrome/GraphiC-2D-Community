! GraphiC ISO C Binding Interface Module
! Standard Fortran 2003/2008/2018/2023 C interoperability layer
module graphic_iso
  use, intrinsic :: iso_c_binding
  implicit none

  abstract interface
    function gpc_funq(x, y) bind(C) result(res)
      import :: c_float
      real(kind=c_float), value :: x, y
      real(kind=c_float) :: res
    end function gpc_funq
  end interface

  interface
    subroutine c_bgnplot(mp, sc, filename) bind(C, name="bgnplot")
      import :: c_char, c_ptr
      character(kind=c_char), value :: mp, sc
      type(c_ptr), value :: filename
    end subroutine c_bgnplot

    subroutine c_startplot(bcolor) bind(C, name="startplot")
      import :: c_int
      integer(kind=c_int), value :: bcolor
    end subroutine c_startplot

    function c_endplot() result(res) bind(C, name="endplot")
      import :: c_int
      integer(kind=c_int) :: res
    end function c_endplot

    subroutine c_stopplot() bind(C, name="stopplot")
    end subroutine c_stopplot

    subroutine c_hardcopy(mode) bind(C, name="hardcopy")
      use, intrinsic :: iso_c_binding, only: c_char
      character(kind=c_char), value :: mode
    end subroutine c_hardcopy

    subroutine c_page(width, height) bind(C, name="page")
      import :: c_float
      real(kind=c_float), value :: width, height
    end subroutine c_page

    subroutine c_area2d(xinch, yinch) bind(C, name="area2d")
      import :: c_float
      real(kind=c_float), value :: xinch, yinch
    end subroutine c_area2d

    subroutine c_svgcrop(margin) bind(C, name="svgcrop")
      import :: c_float
      real(kind=c_float), value :: margin
    end subroutine c_svgcrop

    function c_svgopen(filepath) bind(C, name="svgopen")
      use, intrinsic :: iso_c_binding, only: c_char, c_int
      character(kind=c_char), dimension(*), intent(in) :: filepath
      integer(c_int) :: c_svgopen
    end function c_svgopen

    function c_svgpush(url, filepath) bind(C, name="svgpush")
      use, intrinsic :: iso_c_binding, only: c_char, c_int
      character(kind=c_char), dimension(*), intent(in) :: url, filepath
      integer(c_int) :: c_svgpush
    end function c_svgpush

    subroutine c_physor(x, y) bind(C, name="physor")
      import :: c_float
      real(kind=c_float), value :: x, y
    end subroutine c_physor

    subroutine c_orel(dx, dy) bind(C, name="orel")
      import :: c_float
      real(kind=c_float), value :: dx, dy
    end subroutine c_orel

    subroutine c_box() bind(C, name="box")
    end subroutine c_box

    subroutine c_color(col) bind(C, name="color")
      import :: c_int
      integer(kind=c_int), value :: col
    end subroutine c_color

    subroutine c_dashf(style) bind(C, name="dashf")
      import :: c_int
      integer(kind=c_int), value :: style
    end subroutine c_dashf

    subroutine c_sympick(sym) bind(C, name="sympick")
      import :: c_int
      integer(kind=c_int), value :: sym
    end subroutine c_sympick

    subroutine c_graf(xform, xmin, xstep, xmax, yform, ymin, ystep, ymax) bind(C, name="graf")
      import :: c_ptr, c_float
      type(c_ptr), value :: xform, yform
      real(kind=c_float), value :: xmin, xstep, xmax, ymin, ystep, ymax
    end subroutine c_graf

    subroutine c_heading(title) bind(C, name="heading")
      import :: c_ptr
      type(c_ptr), value :: title
    end subroutine c_heading

    subroutine c_xname(xlab) bind(C, name="xname")
      import :: c_ptr
      type(c_ptr), value :: xlab
    end subroutine c_xname

    subroutine c_yname(ylab) bind(C, name="yname")
      import :: c_ptr
      type(c_ptr), value :: ylab
    end subroutine c_yname

    subroutine c_curve(x, y, npts, symbol) bind(C, name="curve")
      import :: c_float, c_int
      real(kind=c_float), dimension(*) :: x, y
      integer(kind=c_int), value :: npts, symbol
    end subroutine c_curve

    subroutine c_scales(nxdiv, nydiv, x, y, npts) bind(C, name="scales")
      import :: c_float, c_int
      integer(kind=c_int), value :: nxdiv, nydiv, npts
      real(kind=c_float), dimension(*) :: x, y
    end subroutine c_scales

    subroutine c_volm3d(xlen, ylen, zlen) bind(C, name="volm3d")
      import :: c_float
      real(kind=c_float), value :: xlen, ylen, zlen
    end subroutine c_volm3d

    subroutine c_vuabs(x, y, z) bind(C, name="vuabs")
      import :: c_float
      real(kind=c_float), value :: x, y, z
    end subroutine c_vuabs

    subroutine c_graf3d(xmin, xstep, xmax, ymin, ystep, ymax, zmin, zstep, zmax) bind(C, name="graf3d")
      import :: c_float
      real(kind=c_float), value :: xmin, xstep, xmax, ymin, ystep, ymax, zmin, zstep, zmax
    end subroutine c_graf3d

    subroutine c_surmat(zmat, nx, ny) bind(C, name="surmat")
      import :: c_float, c_int
      real(kind=c_float), dimension(*) :: zmat
      integer(kind=c_int), value :: nx, ny
    end subroutine c_surmat

    subroutine c_survis(opt) bind(C, name="survis")
      import :: c_ptr
      type(c_ptr), value :: opt
    end subroutine c_survis

    subroutine c_d3bars(bwid) bind(C, name="d3bars")
      import :: c_float
      real(kind=c_float), value :: bwid
    end subroutine c_d3bars

    subroutine c_conmat(zmat, x, nx, y, ny, orig, step, maxq, styles, labint) bind(C, name="conmat")
      import :: c_float, c_int, c_ptr
      real(kind=c_float), dimension(*) :: zmat, x, y
      integer(kind=c_int), value :: nx, ny, labint
      real(kind=c_float), value :: orig, step, maxq
      type(c_ptr), value :: styles
    end subroutine c_conmat

    subroutine c_smdraw(grid_col) bind(C, name="smdraw")
      import :: c_int
      integer(kind=c_int), value :: grid_col
    end subroutine c_smdraw

    subroutine c_plotrx(r, x) bind(C, name="plotrx")
      import :: c_float
      real(kind=c_float), value :: r, x
    end subroutine c_plotrx

    subroutine c_polgrid(rmax, rmin, mode, larray, dim, ntheta, col) bind(C, name="polgrid")
      import :: c_float, c_int, c_ptr
      real(kind=c_float), value :: rmax, rmin
      integer(kind=c_int), value :: mode, dim, ntheta, col
      type(c_ptr), value :: larray
    end subroutine c_polgrid

    subroutine c_polcurve(angles, radii, npts, symbol) bind(C, name="polcurve")
      import :: c_float, c_int
      real(kind=c_float), dimension(*) :: angles, radii
      integer(kind=c_int), value :: npts, symbol
    end subroutine c_polcurve

    subroutine c_xlog(xmin, xmax, xform, ymin, ystep, ymax) bind(C, name="xlog")
      import :: c_float, c_ptr
      real(kind=c_float), value :: xmin, xmax, ymin, ystep, ymax
      type(c_ptr), value :: xform
    end subroutine c_xlog

    subroutine c_ylog(yform, xmin, xstep, xmax, ymin, ymax) bind(C, name="ylog")
      import :: c_float, c_ptr
      real(kind=c_float), value :: xmin, xstep, xmax, ymin, ymax
      type(c_ptr), value :: yform
    end subroutine c_ylog

    subroutine c_uvector(x1, y1, x2, y2, head, sz, opt) bind(C, name="uvector")
      import :: c_float, c_ptr
      real(kind=c_float), value :: x1, y1, x2, y2, head, sz
      type(c_ptr), value :: opt
    end subroutine c_uvector

    subroutine c_vector(x1, y1, x2, y2, head, sz, opt) bind(C, name="vector")
      import :: c_float, c_ptr
      real(kind=c_float), value :: x1, y1, x2, y2, head, sz
      type(c_ptr), value :: opt
    end subroutine c_vector

    subroutine c_uline(xf, yf, xt, yt, newcurv) bind(C, name="uline")
      import :: c_float, c_int
      real(kind=c_float), value :: xf, yf, xt, yt
      integer(kind=c_int), value :: newcurv
    end subroutine c_uline

    subroutine c_linch(x1, y1, x2, y2, newcurv) bind(C, name="linch")
      import :: c_float, c_char
      real(kind=c_float), value :: x1, y1, x2, y2
      character(kind=c_char), value :: newcurv
    end subroutine c_linch

    subroutine c_line(xf, yf, xt, yt, newcurv) bind(C, name="line")
      import :: c_int
      integer(kind=c_int), value :: xf, yf, xt, yt, newcurv
    end subroutine c_line

    subroutine c_axesoff() bind(C, name="axesoff")
    end subroutine c_axesoff

    subroutine c_pbox() bind(C, name="pbox")
    end subroutine c_pbox

    subroutine c_aname(name) bind(C, name="aname")
      import :: c_ptr
      type(c_ptr), value :: name
    end subroutine c_aname

    subroutine c_bname(name) bind(C, name="bname")
      import :: c_ptr
      type(c_ptr), value :: name
    end subroutine c_bname

    subroutine c_cname(name) bind(C, name="cname")
      import :: c_ptr
      type(c_ptr), value :: name
    end subroutine c_cname

    subroutine c_bar(nxdiv, x, y, npts, bwid, mode, car, tpe) bind(C, name="bar")
      import :: c_int, c_float, c_ptr
      integer(kind=c_int), value :: nxdiv, npts, mode
      real(kind=c_float), dimension(*) :: x, y
      real(kind=c_float), value :: bwid
      type(c_ptr), value :: car, tpe
    end subroutine c_bar

    subroutine c_pieplot(nslice, seg, clr, offset, cap, size, border, txtclr) bind(C, name="pieplot")
      import :: c_int, c_float, c_ptr
      integer(kind=c_int), value :: nslice, border, txtclr
      real(kind=c_float), dimension(*) :: seg, offset
      integer(kind=c_int), dimension(*) :: clr
      type(c_ptr), value :: cap
      real(kind=c_float), value :: size
    end subroutine c_pieplot

    subroutine c_panelinch(x, y, nlines, pcolor, border) bind(C, name="panelinch")
      import :: c_float, c_int
      real(kind=c_float), dimension(*) :: x, y
      integer(kind=c_int), value :: nlines, pcolor, border
    end subroutine c_panelinch

    subroutine c_panproc() bind(C, name="panproc")
    end subroutine c_panproc

    subroutine c_pltfnt(x, y, str, height, angle) bind(C, name="pltfnt")
      import :: c_float, c_int, c_ptr
      real(kind=c_float), value :: x, y, height
      type(c_ptr), value :: str
      integer(kind=c_int), value :: angle
    end subroutine c_pltfnt

    subroutine c_prtfnt(xinch, yinch, str, height, angle) bind(C, name="prtfnt")
      import :: c_float, c_int, c_ptr
      real(kind=c_float), value :: xinch, yinch, height
      type(c_ptr), value :: str
      integer(kind=c_int), value :: angle
    end subroutine c_prtfnt

    subroutine c_simuplot(numplots) bind(C, name="simuplot")
      import :: c_int
      integer(kind=c_int), value :: numplots
    end subroutine c_simuplot

    subroutine c_context(ctx) bind(C, name="context")
      import :: c_int
      integer(kind=c_int), value :: ctx
    end subroutine c_context

    subroutine c_cross(iscross) bind(C, name="cross")
      import :: c_char
      character(kind=c_char), value :: iscross
    end subroutine c_cross

    subroutine c_CrossAt(x, y) bind(C, name="CrossAt")
      import :: c_float
      real(kind=c_float), value :: x, y
    end subroutine c_CrossAt

    subroutine c_frame(frameon, ftick) bind(C, name="frame")
      import :: c_char
      character(kind=c_char), value :: frameon, ftick
    end subroutine c_frame

    subroutine c_tickht(ht) bind(C, name="tickht")
      import :: c_float
      real(kind=c_float), value :: ht
    end subroutine c_tickht

    subroutine c_tickout(is_out) bind(C, name="tickout")
      import :: c_int
      integer(kind=c_int), value :: is_out
    end subroutine c_tickout

    subroutine c_numht(ht) bind(C, name="numht")
      import :: c_float
      real(kind=c_float), value :: ht
    end subroutine c_numht

    subroutine c_titlht(ht) bind(C, name="titlht")
      import :: c_float
      real(kind=c_float), value :: ht
    end subroutine c_titlht

    subroutine c_axnamht(ht) bind(C, name="axnamht")
      import :: c_float
      real(kind=c_float), value :: ht
    end subroutine c_axnamht

    subroutine c_upright(uplab) bind(C, name="upright")
      import :: c_char
      character(kind=c_char), value :: uplab
    end subroutine c_upright

    subroutine c_acrop(flag) bind(C, name="acrop")
      import :: c_char
      character(kind=c_char), value :: flag
    end subroutine c_acrop

    subroutine c_rotate(isrot) bind(C, name="rotate")
      import :: c_char
      character(kind=c_char), value :: isrot
    end subroutine c_rotate

    subroutine c_pgshift(x, y) bind(C, name="pgshift")
      import :: c_float
      real(kind=c_float), value :: x, y
    end subroutine c_pgshift

    subroutine c_grid(kind) bind(C, name="grid")
      import :: c_int
      integer(kind=c_int), value :: kind
    end subroutine c_grid

    subroutine c_fgrid(fon, ndiv) bind(C, name="fgrid")
      import :: c_int
      integer(kind=c_int), value :: fon, ndiv
    end subroutine c_fgrid

    subroutine c_xfgrid(fon, nx) bind(C, name="xfgrid")
      import :: c_int
      integer(kind=c_int), value :: fon, nx
    end subroutine c_xfgrid

    subroutine c_yfgrid(fon, ny) bind(C, name="yfgrid")
      import :: c_int
      integer(kind=c_int), value :: fon, ny
    end subroutine c_yfgrid

    subroutine c_box3d() bind(C, name="box3d")
    end subroutine c_box3d

    subroutine c_d3line(xf, yf, zf, xt, yt, zt) bind(C, name="d3line")
      import :: c_float
      real(kind=c_float), value :: xf, yf, zf, xt, yt, zt
    end subroutine c_d3line

    subroutine c_curv3d(pa, pb, pc, npnts) bind(C, name="curv3d")
      import :: c_float, c_int
      real(kind=c_float), dimension(*) :: pa, pb, pc
      integer(kind=c_int), value :: npnts
    end subroutine c_curv3d

    subroutine c_d3color(array, dim, mesh) bind(C, name="d3color")
      import :: c_int, c_char
      integer(kind=c_int), dimension(*) :: array
      integer(kind=c_int), value :: dim
      character(kind=c_char), value :: mesh
    end subroutine c_d3color

    subroutine c_bcolor(val) bind(C, name="bcolor")
      import :: c_char
      character(kind=c_char), value :: val
    end subroutine c_bcolor

    subroutine c_tcolor(val) bind(C, name="tcolor")
      import :: c_char
      character(kind=c_char), value :: val
    end subroutine c_tcolor

    subroutine c_d3head(title, place) bind(C, name="d3head")
      import :: c_ptr, c_int
      type(c_ptr), value :: title
      integer(kind=c_int), value :: place
    end subroutine c_d3head

    subroutine c_x3name(name) bind(C, name="x3name")
      import :: c_ptr
      type(c_ptr), value :: name
    end subroutine c_x3name

    subroutine c_y3name(name) bind(C, name="y3name")
      import :: c_ptr
      type(c_ptr), value :: name
    end subroutine c_y3name

    subroutine c_z3name(name) bind(C, name="z3name")
      import :: c_ptr
      type(c_ptr), value :: name
    end subroutine c_z3name

    subroutine c_sclfix(isfixed) bind(C, name="sclfix")
      import :: c_int
      integer(kind=c_int), value :: isfixed
    end subroutine c_sclfix

    subroutine c_nohide(visible) bind(C, name="nohide")
      import :: c_int
      integer(kind=c_int), value :: visible
    end subroutine c_nohide

    subroutine c_vuangl(phi, theta, radius) bind(C, name="vuangl")
      import :: c_float
      real(kind=c_float), value :: phi, theta, radius
    end subroutine c_vuangl

    subroutine c_contcolor_fixed(colors, bord, style) bind(C, name="contcolor_fixed")
      import :: c_int, c_ptr
      integer(kind=c_int), dimension(*) :: colors
      integer(kind=c_int), value :: bord
      type(c_ptr), value :: style
    end subroutine c_contcolor_fixed

    subroutine c_curvfill(x, y, npts, x1, y1, npts1, pcolor, border) bind(C, name="curvfill")
      import :: c_float, c_int
      real(kind=c_float), dimension(*) :: x, y, x1, y1
      integer(kind=c_int), value :: npts, npts1, pcolor, border
    end subroutine c_curvfill

    subroutine c_errorbar(x, xmin, xmax, y, ymin, ymax) bind(C, name="errorbar")
      import :: c_float
      real(kind=c_float), value :: x, xmin, xmax, y, ymin, ymax
    end subroutine c_errorbar

    subroutine c_scatplot(dot) bind(C, name="scatplot")
      import :: c_char
      character(kind=c_char), value :: dot
    end subroutine c_scatplot

    subroutine c_histogram(x, y, npts, type_val, colors) bind(C, name="histogram")
      import :: c_float, c_int, c_ptr
      real(kind=c_float), dimension(*) :: x, y
      integer(kind=c_int), value :: npts, type_val
      type(c_ptr), value :: colors
    end subroutine c_histogram

    subroutine c_ctline(str, height) bind(C, name="ctline")
      import :: c_ptr, c_float
      type(c_ptr), value :: str
      real(kind=c_float), value :: height
    end subroutine c_ctline

    subroutine c_ltline(str, height) bind(C, name="ltline")
      import :: c_ptr, c_float
      type(c_ptr), value :: str
      real(kind=c_float), value :: height
    end subroutine c_ltline

    subroutine c_rtline(str, height) bind(C, name="rtline")
      import :: c_ptr, c_float
      type(c_ptr), value :: str
      real(kind=c_float), value :: height
    end subroutine c_rtline

    function c_strwidth(str, height) result(w) bind(C, name="strwidth")
      import :: c_ptr, c_float
      type(c_ptr), value :: str
      real(kind=c_float), value :: height
      real(kind=c_float) :: w
    end function c_strwidth

    subroutine c_subsym(ch) bind(C, name="subsym")
      import :: c_char
      character(kind=c_char), value :: ch
    end subroutine c_subsym

    subroutine c_legend(marker, str, style, strhgt, scolor) bind(C, name="legend")
      import :: c_int, c_ptr, c_float
      integer(kind=c_int), value :: marker, style, scolor
      type(c_ptr), value :: str
      real(kind=c_float), value :: strhgt
    end subroutine c_legend

    subroutine c_legpos(entries, xleg, yleg, border) bind(C, name="legpos")
      import :: c_int, c_float
      integer(kind=c_int), value :: entries, border
      real(kind=c_float), value :: xleg, yleg
    end subroutine c_legpos

    subroutine c_lglgscle(x, y, npts) bind(C, name="lglgscle")
      import :: c_float, c_int
      real(kind=c_float), dimension(*) :: x, y
      integer(kind=c_int), value :: npts
    end subroutine c_lglgscle

    subroutine c_loglog(xorig, xmax, yorig, ymax) bind(C, name="loglog")
      import :: c_float
      real(kind=c_float), value :: xorig, xmax, yorig, ymax
    end subroutine c_loglog

    subroutine c_wedge(rmin, rmax, thmin, thmax, pattern, border) bind(C, name="wedge")
      import :: c_float, c_int
      real(kind=c_float), value :: rmin, rmax, thmin, thmax
      integer(kind=c_int), value :: pattern, border
    end subroutine c_wedge

    subroutine c_stack(stacked) bind(C, name="stack")
      import :: c_char
      character(kind=c_char), value :: stacked
    end subroutine c_stack

    subroutine c_plotrho(rho, theta) bind(C, name="plotrho")
      import :: c_float
      real(kind=c_float), value :: rho, theta
    end subroutine c_plotrho

    subroutine c_smithcir(u, v, radius) bind(C, name="smithcir")
      import :: c_float
      real(kind=c_float), value :: u, v, radius
    end subroutine c_smithcir

    subroutine c_smove(dell) bind(C, name="smove")
      import :: c_float
      real(kind=c_float), value :: dell
    end subroutine c_smove

    subroutine c_tline(width) bind(C, name="tline")
      import :: c_int
      integer(kind=c_int), value :: width
    end subroutine c_tline

    subroutine c_tcurve(width) bind(C, name="tcurve")
      import :: c_int
      integer(kind=c_int), value :: width
    end subroutine c_tcurve

    subroutine c_plotadd() bind(C, name="plotadd")
    end subroutine c_plotadd

    subroutine c_plotabsolute(flag) bind(C, name="plotabsolute")
      import :: c_int
      integer(kind=c_int), value :: flag
    end subroutine c_plotabsolute

    subroutine c_NewFile(name) bind(C, name="NewFile")
      import :: c_ptr
      type(c_ptr), value :: name
    end subroutine c_NewFile

    subroutine c_dateit(font) bind(C, name="dateit")
      import :: c_char
      character(kind=c_char), value :: font
    end subroutine c_dateit

    subroutine c_linesp(ratio) bind(C, name="linesp")
      import :: c_float
      real(kind=c_float), value :: ratio
    end subroutine c_linesp

    subroutine c_lmargin(marginch) bind(C, name="lmargin")
      import :: c_float
      real(kind=c_float), value :: marginch
    end subroutine c_lmargin

    subroutine c_rmargin(marginch) bind(C, name="rmargin")
      import :: c_float
      real(kind=c_float), value :: marginch
    end subroutine c_rmargin

    subroutine c_tmargin(marginch) bind(C, name="tmargin")
      import :: c_float
      real(kind=c_float), value :: marginch
    end subroutine c_tmargin

    subroutine c_charspc(width) bind(C, name="charspc")
      import :: c_int
      integer(kind=c_int), value :: width
    end subroutine c_charspc

    subroutine c_skew(slant) bind(C, name="skew")
      import :: c_float
      real(kind=c_float), value :: slant
    end subroutine c_skew

    subroutine c_widen(fact) bind(C, name="widen")
      import :: c_float
      real(kind=c_float), value :: fact
    end subroutine c_widen

    subroutine c_tfont(wid) bind(C, name="tfont")
      import :: c_int
      integer(kind=c_int), value :: wid
    end subroutine c_tfont

    subroutine c_tifont(wid) bind(C, name="tifont")
      import :: c_float
      real(kind=c_float), value :: wid
    end subroutine c_tifont

    subroutine c_fntchg(sym) bind(C, name="fntchg")
      import :: c_char
      character(kind=c_char), value :: sym
    end subroutine c_fntchg

    subroutine c_supsym(ch) bind(C, name="supsym")
      import :: c_char
      character(kind=c_char), value :: ch
    end subroutine c_supsym

    subroutine c_putlabel(string, xpos, ypos, height, border, units) bind(C, name="putlabel")
      import :: c_ptr, c_float, c_int
      type(c_ptr), value :: string
      real(kind=c_float), value :: xpos, ypos, height
      integer(kind=c_int), value :: border, units
    end subroutine c_putlabel

    subroutine c_d3base(isbase, colorb) bind(C, name="d3base")
      import :: c_int
      integer(kind=c_int), value :: isbase, colorb
    end subroutine c_d3base

    subroutine c_fillfont(fill) bind(C, name="fillfont")
      import :: c_int
      integer(kind=c_int), value :: fill
    end subroutine c_fillfont

    subroutine c_fontfill(fill, pattern, border) bind(C, name="fontfill")
      import :: c_int
      integer(kind=c_int), value :: fill, pattern, border
    end subroutine c_fontfill
  
    ! --- Full Parity Additions ---
    subroutine c_boxplot(x, y, ydim, outlier, symnum, width) bind(C, name="BoxPlot")
      import :: c_float, c_int, c_ptr
      real(kind=c_float), value :: x, width
      type(c_ptr), value :: y
      integer(kind=c_int), value :: ydim, outlier, symnum
    end subroutine c_boxplot

    subroutine c_waterfallplot(x, y, z, nx, ny) bind(C, name="WaterfallPlot")
      import :: c_ptr, c_int
      type(c_ptr), value :: x, y, z
      integer(kind=c_int), value :: nx, ny
    end subroutine c_waterfallplot

    subroutine c_waterfallcurve(x, y, z, npts) bind(C, name="WaterfallCurve")
      import :: c_ptr, c_float, c_int
      type(c_ptr), value :: x, z
      real(kind=c_float), value :: y
      integer(kind=c_int), value :: npts
    end subroutine c_waterfallcurve

    subroutine c_plane3d(wplane, pos, hide) bind(C, name="plane3d")
      import :: c_char, c_float
      character(kind=c_char), value :: wplane, hide
      real(kind=c_float), value :: pos
    end subroutine c_plane3d

    subroutine c_colorkeyl(minv, maxv, intensity, nintensity, legllx, leglly, &
                           legurx, legury, labint, format, fontc, height, angle) &
                           bind(C, name="ColorKeyL")
      import :: c_float, c_int, c_ptr, c_char
      real(kind=c_float), value :: minv, maxv, legllx, leglly, legurx, legury, height
      type(c_ptr), value :: intensity, format
      integer(kind=c_int), value :: nintensity, labint, angle
      character(kind=c_char), value :: fontc
    end subroutine c_colorkeyl

    subroutine c_erfcurve(x, y, npts, sym) bind(C, name="erfcurve")
      import :: c_ptr, c_int
      type(c_ptr), value :: x, y
      integer(kind=c_int), value :: npts, sym
    end subroutine c_erfcurve

    subroutine c_erfgrid(ymin, ystep, ymax, yformat) bind(C, name="erfgrid")
      import :: c_float, c_ptr
      real(kind=c_float), value :: ymin, ystep, ymax
      type(c_ptr), value :: yformat
    end subroutine c_erfgrid

    subroutine c_erfscales(nydiv, y, npts) bind(C, name="erfscales")
      import :: c_int, c_ptr
      integer(kind=c_int), value :: nydiv, npts
      type(c_ptr), value :: y
    end subroutine c_erfscales

    subroutine c_usertoinch(x_in, y_in, x_out, y_out) bind(C, name="usertoinch")
      import :: c_float, c_ptr
      real(kind=c_float), value :: x_in, y_in
      type(c_ptr), value :: x_out, y_out
    end subroutine c_usertoinch

    subroutine c_usertopix(x_in, y_in, z_in, x_out, y_out) bind(C, name="usertopix")
      import :: c_float, c_ptr
      real(kind=c_float), value :: x_in, y_in, z_in
      type(c_ptr), value :: x_out, y_out
    end subroutine c_usertopix

    function c_intopix(inches) result(res) bind(C, name="intopix")
      import :: c_float, c_int
      real(kind=c_float), value :: inches
      integer(kind=c_int) :: res
    end function c_intopix

    function c_pixtoin(pixels) result(res) bind(C, name="pixtoin")
      import :: c_float, c_int
      integer(kind=c_int), value :: pixels
      real(kind=c_float) :: res
    end function c_pixtoin

    subroutine c_ellipse(xc, yc, xr, yr, rotation, th1, th2, degrees, units) bind(C, name="ellipse")
      import :: c_float, c_char, c_int
      real(kind=c_float), value :: xc, yc, xr, yr, rotation, th1, th2
      character(kind=c_char), value :: degrees
      integer(kind=c_int), value :: units
    end subroutine c_ellipse

    subroutine c_fillellipse(onoff, pattern, border) bind(C, name="fillellipse")
      import :: c_int
      integer(kind=c_int), value :: onoff, pattern, border
    end subroutine c_fillellipse

    subroutine c_blank(attr) bind(C, name="blank")
      import :: c_int
      integer(kind=c_int), value :: attr
    end subroutine c_blank

    subroutine c_d3grid(subdiv, label) bind(C, name="d3grid")
      import :: c_int
      integer(kind=c_int), value :: subdiv, label
    end subroutine c_d3grid

    subroutine c_d3pltfnt(x, y, z, string, height, angle) bind(C, name="d3pltfnt")
      import :: c_float, c_ptr, c_int
      real(kind=c_float), value :: x, y, z, height
      type(c_ptr), value :: string
      integer(kind=c_int), value :: angle
    end subroutine c_d3pltfnt

    subroutine c_hlstorgb(h, l, s, r, g, b) bind(C, name="HLStoRGB")
      import :: c_int, c_ptr
      integer(kind=c_int), value :: h, l, s
      type(c_ptr), value :: r, g, b
    end subroutine c_hlstorgb

    subroutine c_rgbtohls(r, g, b, h, l, s) bind(C, name="RGBtoHLS")
      import :: c_int, c_ptr
      integer(kind=c_int), value :: r, g, b
      type(c_ptr), value :: h, l, s
    end subroutine c_rgbtohls

    function c_mean(x, n) result(res) bind(C, name="mean")
      import :: c_float, c_int, c_ptr
      type(c_ptr), value :: x
      integer(kind=c_int), value :: n
      real(kind=c_float) :: res
    end function c_mean

    function c_median(x, n) result(res) bind(C, name="median")
      import :: c_float, c_int, c_ptr
      type(c_ptr), value :: x
      integer(kind=c_int), value :: n
      real(kind=c_float) :: res
    end function c_median

    function c_stddev(x, n) result(res) bind(C, name="StdDev")
      import :: c_float, c_int, c_ptr
      type(c_ptr), value :: x
      integer(kind=c_int), value :: n
      real(kind=c_float) :: res
    end function c_stddev

    subroutine c_minmax(minq, maxq, xy, npts) bind(C, name="minmax")
      import :: c_float, c_int, c_ptr
      type(c_ptr), value :: minq, maxq, xy
      integer(kind=c_int), value :: npts
    end subroutine c_minmax

    subroutine c_setttfull(onoff, str) bind(C, name="SetTTFull")
      import :: c_char
      character(kind=c_char), value :: onoff, str
    end subroutine c_setttfull

    subroutine c_setttkerning(onoff, str) bind(C, name="SetTTKerning")
      import :: c_char
      character(kind=c_char), value :: onoff, str
    end subroutine c_setttkerning

    subroutine c_setttmaxerror(err, str) bind(C, name="SetTTMaxError")
      import :: c_float, c_char
      real(kind=c_float), value :: err
      character(kind=c_char), value :: str
    end subroutine c_setttmaxerror

    subroutine c_setttusememory(onoff) bind(C, name="SetTTUseMemory")
      import :: c_char
      character(kind=c_char), value :: onoff
    end subroutine c_setttusememory

    subroutine c_settolerance(tolerance) bind(C, name="settolerance")
      import :: c_float
      real(kind=c_float), value :: tolerance
    end subroutine c_settolerance

    subroutine c_saveplotq() bind(C, name="saveplotq")
    end subroutine c_saveplotq

  
    subroutine c_pixtouser(x_in, y_in, x_out, y_out) bind(C, name="pixtouser")
      import :: c_int, c_ptr
      integer(kind=c_int), value :: x_in, y_in
      type(c_ptr), value :: x_out, y_out
    end subroutine c_pixtouser

    subroutine c_gpcbeep(freq, duration) bind(C, name="gpcBeep")
      import :: c_int
      integer(kind=c_int), value :: freq, duration
    end subroutine c_gpcbeep

    subroutine c_set_time(timer_id) bind(C, name="set_time")
      import :: c_int
      integer(kind=c_int), value :: timer_id
    end subroutine c_set_time

    subroutine c_trimat(m1, m2, z, dim, dim1, zmin, zstep, zmax, lnstyle, border) bind(C, name="trimat")
      import :: c_ptr, c_int, c_float
      type(c_ptr), value :: m1, m2, z
      integer(kind=c_int), value :: dim, dim1
      real(kind=c_float), value :: zmin, zstep, zmax
      type(c_ptr), value :: lnstyle
      integer(kind=c_int), value :: border
    end subroutine c_trimat

    subroutine c_trifun(fun, dim, zmin, zstep, zmax, lnstyle, border) bind(C, name="trifun")
      import :: c_funptr, c_int, c_float, c_ptr
      type(c_funptr), value :: fun
      integer(kind=c_int), value :: dim
      real(kind=c_float), value :: zmin, zstep, zmax
      type(c_ptr), value :: lnstyle
      integer(kind=c_int), value :: border
    end subroutine c_trifun

    subroutine c_surmat2(zmat, nx, ny) bind(C, name="surmat2")
      import :: c_ptr, c_int
      type(c_ptr), value :: zmat
      integer(kind=c_int), value :: nx, ny
    end subroutine c_surmat2

    subroutine c_surfun(fun, ixpts, xdel, iypts, ydel) bind(C, name="surfun")
      import :: c_funptr, c_int, c_float
      type(c_funptr), value :: fun
      integer(kind=c_int), value :: ixpts
      real(kind=c_float), value :: xdel
      integer(kind=c_int), value :: iypts
      real(kind=c_float), value :: ydel
    end subroutine c_surfun

    subroutine c_spline(x, y, n, n1, s1, s2, xn, yn) bind(C, name="spline")
      import :: c_ptr, c_int, c_float
      type(c_ptr), value :: x, y
      integer(kind=c_int), value :: n, n1
      real(kind=c_float), value :: s1, s2
      type(c_ptr), value :: xn, yn
    end subroutine c_spline

    function c_gpc_rgb(r, g, b) result(res) bind(C, name="gpc_rgb")
      import :: c_int
      integer(kind=c_int), value :: r, g, b
      integer(kind=c_int) :: res
    end function c_gpc_rgb

    function c_gpc_colormap_eval(cmap_id, t) result(res) bind(C, name="gpc_colormap_eval")
      import :: c_int, c_float
      integer(kind=c_int), value :: cmap_id
      real(kind=c_float), value :: t
      integer(kind=c_int) :: res
    end function c_gpc_colormap_eval

    function c_gpc_get_color(cmap_id, val, minv, maxv) result(res) bind(C, name="gpc_get_color")
      import :: c_int, c_float
      integer(kind=c_int), value :: cmap_id
      real(kind=c_float), value :: val, minv, maxv
      integer(kind=c_int) :: res
    end function c_gpc_get_color

    subroutine c_gpc_get_rgb(cmap_id, val, minv, maxv, r, g, b) bind(C, name="gpc_get_rgb")
      import :: c_int, c_float, c_ptr
      integer(kind=c_int), value :: cmap_id
      real(kind=c_float), value :: val, minv, maxv
      type(c_ptr), value :: r, g, b
    end subroutine c_gpc_get_rgb

    subroutine c_gpc_scatter(x, y, z, dim, minv, maxv, symnum, symheight, cmap_id) bind(C, name="gpc_scatter")
      import :: c_ptr, c_int, c_float
      type(c_ptr), value :: x, y, z
      integer(kind=c_int), value :: dim, symnum, cmap_id
      real(kind=c_float), value :: minv, maxv, symheight
    end subroutine c_gpc_scatter

    subroutine c_colorkey_gradient(minv, maxv, cmap_id, legllx, leglly, &
                                   legurx, legury, nticks, format, fontc, &
                                   height, angle) bind(C, name="ColorKeyGradient")
      import :: c_float, c_int, c_ptr, c_char
      real(kind=c_float), value :: minv, maxv, legllx, leglly, legurx, legury, height
      integer(kind=c_int), value :: cmap_id, nticks, angle
      type(c_ptr), value :: format
      character(kind=c_char), value :: fontc
    end subroutine c_colorkey_gradient

    subroutine c_gpc_load_font(fontname, esc) bind(C, name="gpc_load_font")
      import :: c_ptr, c_char
      type(c_ptr), value :: fontname
      character(kind=c_char), value :: esc
    end subroutine c_gpc_load_font
  end interface
end module graphic_iso
