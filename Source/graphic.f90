! GraphiC High-Level Idiomatic Fortran Module
! Provides convenient wrappers, automatic string null-termination, and constants.
module graphic
  use graphic_iso
  use, intrinsic :: iso_c_binding
  implicit none

  ! Standard Color Indices
  integer(c_int), parameter :: COLOR_BLACK     = 0_c_int
  integer(c_int), parameter :: COLOR_BLUE      = 1_c_int
  integer(c_int), parameter :: COLOR_GREEN     = 2_c_int
  integer(c_int), parameter :: COLOR_CYAN      = 3_c_int
  integer(c_int), parameter :: COLOR_RED       = 4_c_int
  integer(c_int), parameter :: COLOR_MAGENTA   = 5_c_int
  integer(c_int), parameter :: COLOR_BROWN     = 6_c_int
  integer(c_int), parameter :: COLOR_WHITE     = 7_c_int
  integer(c_int), parameter :: COLOR_GRAY      = 8_c_int
  integer(c_int), parameter :: COLOR_LTBLUE    = 9_c_int
  integer(c_int), parameter :: COLOR_LTGREEN   = 10_c_int
  integer(c_int), parameter :: COLOR_LTCYAN    = 11_c_int
  integer(c_int), parameter :: COLOR_LTRED     = 12_c_int
  integer(c_int), parameter :: COLOR_LTMAGENTA = 13_c_int
  integer(c_int), parameter :: COLOR_YELLOW    = 14_c_int
  integer(c_int), parameter :: COLOR_BRWHITE   = 15_c_int

  ! Line Style Constants
  integer(c_int), parameter :: STYLE_SOLID     = 0_c_int
  integer(c_int), parameter :: STYLE_DOTTED    = 1_c_int
  integer(c_int), parameter :: STYLE_DASHED    = 2_c_int
  integer(c_int), parameter :: STYLE_DOTDASH   = 3_c_int

  ! Driver Codes
  character(kind=c_char), parameter :: DRIVER_SVG = 'S'
  character(kind=c_char), parameter :: DRIVER_TEK = 'T'
  character(len=512) :: last_tkf = ''

  ! Scientific Colormaps
  integer(c_int), parameter :: GPC_COLORMAP_VIRIDIS  = 0_c_int
  integer(c_int), parameter :: GPC_COLORMAP_PLASMA   = 1_c_int
  integer(c_int), parameter :: GPC_COLORMAP_INFERNO  = 2_c_int
  integer(c_int), parameter :: GPC_COLORMAP_MAGMA    = 3_c_int
  integer(c_int), parameter :: GPC_COLORMAP_COOLWARM = 4_c_int
  integer(c_int), parameter :: GPC_COLORMAP_RAINBOW  = 5_c_int
  integer(c_int), parameter :: GPC_COLORMAP_TURBO    = 6_c_int

contains

  ! Initialize plot session
  subroutine gpc_begin(driver, filename)
    character(kind=c_char), intent(in) :: driver
    character(len=*), intent(in) :: filename
    character(kind=c_char, len=:), allocatable, target :: c_file
    character(len=512) :: tkf_name
    integer :: p

    tkf_name = trim(filename)
    p = index(tkf_name, ".svg", back=.true.)
    if (p > 0) then
      tkf_name = tkf_name(1:p-1) // ".tkf"
    end if

    last_tkf = trim(tkf_name)
    c_file = trim(tkf_name) // c_null_char
    call c_bgnplot(c_null_char, driver, c_loc(c_file))
  end subroutine gpc_begin

  subroutine gpc_bgnplot(driver, filename)
    character(kind=c_char), intent(in) :: driver
    character(len=*), intent(in) :: filename
    call gpc_begin(driver, filename)
  end subroutine gpc_bgnplot

  ! Start new plot page with background color
  subroutine gpc_start(bg_color)
    integer(c_int), intent(in) :: bg_color
    call c_startplot(bg_color)
  end subroutine gpc_start

  subroutine gpc_startplot(bg_color)
    integer(c_int), intent(in) :: bg_color
    call gpc_start(bg_color)
  end subroutine gpc_startplot

  ! End current plot
  subroutine gpc_end()
    integer(c_int) :: res
    res = c_endplot()
  end subroutine gpc_end

  subroutine gpc_endplot()
    call gpc_end()
  end subroutine gpc_endplot

  ! Terminate plotting session
  subroutine gpc_stop()
    call c_stopplot()
    if (len_trim(last_tkf) > 0) then
      call execute_command_line("rm -f " // trim(last_tkf))
      last_tkf = ""
    end if
  end subroutine gpc_stop

  subroutine gpc_stopplot()
    call gpc_stop()
  end subroutine gpc_stopplot

  subroutine gpc_hardcopy(mode)
    character(len=*), intent(in) :: mode
    if (len_trim(mode) > 0) then
      call c_hardcopy(mode(1:1))
    else
      call c_hardcopy('n')
    end if
  end subroutine gpc_hardcopy

  ! Set canvas page size in inches
  subroutine gpc_page(width, height)
    real, intent(in) :: width, height
    call c_page(real(width, c_float), real(height, c_float))
  end subroutine gpc_page

  ! Set 2D active plot area in inches
  subroutine gpc_area2d(xlen, ylen)
    real, intent(in) :: xlen, ylen
    call c_area2d(real(xlen, c_float), real(ylen, c_float))
  end subroutine gpc_area2d

  ! Set SVG auto-crop margin in inches (>=0 to enable with margin, <0 to disable)
  subroutine gpc_svgcrop(margin)
    real, intent(in) :: margin
    call c_svgcrop(real(margin, c_float))
  end subroutine gpc_svgcrop

  subroutine gpc_svg_crop(margin)
    real, intent(in) :: margin
    call c_svgcrop(real(margin, c_float))
  end subroutine gpc_svg_crop

  ! Open SVG plot in default browser or viewer
  subroutine gpc_svgopen(filepath)
    character(len=*), intent(in), optional :: filepath
    integer :: rc
    if (present(filepath)) then
       rc = c_svgopen(trim(filepath) // c_null_char)
    else
       rc = c_svgopen(c_null_char)
    end if
  end subroutine gpc_svgopen

  subroutine gpc_svg_open(filepath)
    character(len=*), intent(in), optional :: filepath
    call gpc_svgopen(filepath)
  end subroutine gpc_svg_open

  ! Push SVG plot to HTTP/HTTPS endpoint
  subroutine gpc_svgpush(url, filepath)
    character(len=*), intent(in), optional :: url
    character(len=*), intent(in), optional :: filepath
    integer :: rc
    character(kind=c_char, len=:), allocatable :: u_buf, f_buf

    if (present(url)) then
       u_buf = trim(url) // c_null_char
    else
       u_buf = c_null_char
    end if

    if (present(filepath)) then
       f_buf = trim(filepath) // c_null_char
    else
       f_buf = c_null_char
    end if

    rc = c_svgpush(u_buf, f_buf)
  end subroutine gpc_svgpush

  subroutine gpc_svg_push(url, filepath)
    character(len=*), intent(in), optional :: url
    character(len=*), intent(in), optional :: filepath
    call gpc_svgpush(url, filepath)
  end subroutine gpc_svg_push

  ! Move physical origin in inches
  subroutine gpc_physor(x, y)
    real, intent(in) :: x, y
    call c_physor(real(x, c_float), real(y, c_float))
  end subroutine gpc_physor

  ! Relative origin offset
  subroutine gpc_orel(dx, dy)
    real, intent(in) :: dx, dy
    call c_orel(real(dx, c_float), real(dy, c_float))
  end subroutine gpc_orel

  ! Draw bounding box around plot area
  subroutine gpc_box()
    call c_box()
  end subroutine gpc_box

  ! Set active drawing color
  subroutine gpc_color(col)
    integer(c_int), intent(in) :: col
    call c_color(col)
  end subroutine gpc_color

  ! Set dashed line style
  subroutine gpc_dashf(style)
    integer(c_int), intent(in) :: style
    call c_dashf(style)
  end subroutine gpc_dashf

  ! Set marker symbol (1..15)
  subroutine gpc_sympick(sym)
    integer(c_int), intent(in) :: sym
    call c_sympick(sym)
  end subroutine gpc_sympick

  ! Draw calibrated 2D linear axes
  subroutine gpc_graf(xform, xmin, xstep, xmax, yform, ymin, ystep, ymax)
    character(len=*), intent(in) :: xform, yform
    real, intent(in) :: xmin, xstep, xmax, ymin, ystep, ymax
    character(kind=c_char, len=:), allocatable, target :: c_xf, c_yf

    c_xf = trim(xform) // c_null_char
    c_yf = trim(yform) // c_null_char
    call c_graf(c_loc(c_xf), real(xmin, c_float), real(xstep, c_float), real(xmax, c_float), &
                c_loc(c_yf), real(ymin, c_float), real(ystep, c_float), real(ymax, c_float))
  end subroutine gpc_graf

  ! Set heading title and axis labels
  subroutine gpc_title(top, xlab, ylab)
    character(len=*), intent(in) :: top, xlab, ylab
    character(kind=c_char, len=:), allocatable, target :: c_t, c_x, c_y

    if (len_trim(top) > 0) then
      c_t = trim(top) // c_null_char
      call c_heading(c_loc(c_t))
    end if
    if (len_trim(xlab) > 0) then
      c_x = trim(xlab) // c_null_char
      call c_xname(c_loc(c_x))
    end if
    if (len_trim(ylab) > 0) then
      c_y = trim(ylab) // c_null_char
      call c_yname(c_loc(c_y))
    end if
  end subroutine gpc_title

  subroutine gpc_heading(top)
    character(len=*), intent(in) :: top
    character(kind=c_char, len=:), allocatable, target :: c_t
    c_t = trim(top) // c_null_char
    call c_heading(c_loc(c_t))
  end subroutine gpc_heading

  subroutine gpc_xname(xlab)
    character(len=*), intent(in) :: xlab
    character(kind=c_char, len=:), allocatable, target :: c_x
    c_x = trim(xlab) // c_null_char
    call c_xname(c_loc(c_x))
  end subroutine gpc_xname

  subroutine gpc_yname(ylab)
    character(len=*), intent(in) :: ylab
    character(kind=c_char, len=:), allocatable, target :: c_y
    c_y = trim(ylab) // c_null_char
    call c_yname(c_loc(c_y))
  end subroutine gpc_yname

  ! Draw curve connecting points with optional symbol
  subroutine gpc_curve(x, y, npts, symbol)
    real, dimension(*), intent(in) :: x, y
    integer, intent(in) :: npts
    integer, intent(in), optional :: symbol
    integer(c_int) :: sym
    real(c_float), allocatable :: fx(:), fy(:)
    integer :: i

    sym = 0_c_int
    if (present(symbol)) sym = int(symbol, c_int)

    allocate(fx(npts), fy(npts))
    do i = 1, npts
      fx(i) = real(x(i), c_float)
      fy(i) = real(y(i), c_float)
    end do

    call c_curve(fx, fy, int(npts, c_int), sym)
    deallocate(fx, fy)
  end subroutine gpc_curve

  ! 3D Workbox volume in inches
  subroutine gpc_volm3d(xlen, ylen, zlen)
    real, intent(in) :: xlen, ylen, zlen
    call c_volm3d(real(xlen, c_float), real(ylen, c_float), real(zlen, c_float))
  end subroutine gpc_volm3d

  ! 3D Viewpoint observer coordinates
  subroutine gpc_vuabs(x, y, z)
    real, intent(in) :: x, y, z
    call c_vuabs(real(x, c_float), real(y, c_float), real(z, c_float))
  end subroutine gpc_vuabs

  ! Draw 3D coordinate box
  subroutine gpc_graf3d(xmin, xstep, xmax, ymin, ystep, ymax, zmin, zstep, zmax)
    real, intent(in) :: xmin, xstep, xmax, ymin, ystep, ymax, zmin, zstep, zmax
    call c_graf3d(real(xmin, c_float), real(xstep, c_float), real(xmax, c_float), &
                  real(ymin, c_float), real(ystep, c_float), real(ymax, c_float), &
                  real(zmin, c_float), real(zstep, c_float), real(zmax, c_float))
  end subroutine gpc_graf3d

  ! Render 3D surface mesh with hidden-line elimination
  subroutine gpc_surface(zmat, nx, ny)
    real, dimension(nx, ny), intent(in) :: zmat
    integer, intent(in) :: nx, ny
    real(c_float), allocatable :: flat(:)
    character(kind=c_char, len=:), allocatable, target :: vis
    integer :: i, j, idx

    allocate(flat(nx * ny))
    idx = 1
    do j = 1, ny
      do i = 1, nx
        flat(idx) = real(zmat(i, j), c_float)
        idx = idx + 1
      end do
    end do

    vis = "top" // c_null_char
    call c_survis(c_loc(vis))
    call c_surmat(flat, int(nx, c_int), int(ny, c_int))
    deallocate(flat)
  end subroutine gpc_surface

  subroutine gpc_surmat(zmat, nx, ny)
    real, dimension(nx, ny), intent(in) :: zmat
    integer, intent(in) :: nx, ny
    call gpc_surface(zmat, nx, ny)
  end subroutine gpc_surmat

  ! Draw 2D contour map
  subroutine gpc_contour(zmat, x, nx, y, ny, orig, step, maxq, labint)
    real, dimension(nx, ny), intent(in) :: zmat
    real, dimension(nx), intent(in) :: x
    real, dimension(ny), intent(in) :: y
    integer, intent(in) :: nx, ny
    real, intent(in) :: orig, step, maxq
    integer, intent(in) :: labint
    real(c_float), allocatable :: flat(:), fx(:), fy(:)
    integer :: i, j, idx

    allocate(flat(nx * ny), fx(nx), fy(ny))
    idx = 1
    do j = 1, ny
      do i = 1, nx
        flat(idx) = real(zmat(i, j), c_float)
        idx = idx + 1
      end do
    end do

    do i = 1, nx
      fx(i) = real(x(i), c_float)
    end do
    do j = 1, ny
      fy(j) = real(y(j), c_float)
    end do

    call c_conmat(flat, fx, int(nx, c_int), fy, int(ny, c_int), &
                  real(orig, c_float), real(step, c_float), real(maxq, c_float), &
                  c_null_ptr, int(labint, c_int))
    deallocate(flat, fx, fy)
  end subroutine gpc_contour

  subroutine gpc_conmat(zmat, x, nx, y, ny, orig, step, maxq, labint)
    real, dimension(nx, ny), intent(in) :: zmat
    real, dimension(nx), intent(in) :: x
    real, dimension(ny), intent(in) :: y
    integer, intent(in) :: nx, ny
    real, intent(in) :: orig, step, maxq
    integer, intent(in) :: labint
    call gpc_contour(zmat, x, nx, y, ny, orig, step, maxq, labint)
  end subroutine gpc_conmat

  ! RF Microwave Smith Chart
  subroutine gpc_smith_chart(grid_col)
    integer(c_int), intent(in) :: grid_col
    call c_smdraw(grid_col)
  end subroutine gpc_smith_chart

  subroutine gpc_smdraw(grid_col)
    integer(c_int), intent(in) :: grid_col
    call c_smdraw(grid_col)
  end subroutine gpc_smdraw

  ! Plot impedance locus point (R, X) on Smith Chart
  subroutine gpc_plotrx(r, x)
    real, intent(in) :: r, x
    call c_plotrx(real(r, c_float), real(x, c_float))
  end subroutine gpc_plotrx

  ! Polar grid
  subroutine gpc_polgrid(rmax, rmin, col)
    real, intent(in) :: rmax, rmin
    integer(c_int), intent(in) :: col
    call c_polgrid(real(rmax, c_float), real(rmin, c_float), 0_c_int, c_null_ptr, 3_c_int, 8_c_int, col)
  end subroutine gpc_polgrid

  ! Polar curve
  subroutine gpc_polcurve(angles, radii, npts, symbol)
    real, dimension(*), intent(in) :: angles, radii
    integer, intent(in) :: npts
    integer, intent(in), optional :: symbol
    integer(c_int) :: sym
    real(c_float), allocatable :: fa(:), fr(:)
    integer :: i

    sym = 0_c_int
    if (present(symbol)) sym = int(symbol, c_int)

    allocate(fa(npts), fr(npts))
    do i = 1, npts
      fa(i) = real(angles(i), c_float)
      fr(i) = real(radii(i), c_float)
    end do

    call c_polcurve(fa, fr, int(npts, c_int), sym)
    deallocate(fa, fr)
  end subroutine gpc_polcurve

  ! Logarithmic X axis
  subroutine gpc_xlog(xmin, xmax, xform, ymin, ystep, ymax)
    real, intent(in) :: xmin, xmax, ymin, ystep, ymax
    character(len=*), intent(in) :: xform
    character(kind=c_char, len=:), allocatable, target :: c_xf

    c_xf = trim(xform) // c_null_char
    call c_xlog(real(xmin, c_float), real(xmax, c_float), c_loc(c_xf), &
                real(ymin, c_float), real(ystep, c_float), real(ymax, c_float))
  end subroutine gpc_xlog

  ! Logarithmic Y axis
  subroutine gpc_ylog(yform, xmin, xstep, xmax, ymin, ymax)
    character(len=*), intent(in) :: yform
    real, intent(in) :: xmin, xstep, xmax, ymin, ymax
    character(kind=c_char, len=:), allocatable, target :: c_yf

    c_yf = trim(yform) // c_null_char
    call c_ylog(c_loc(c_yf), real(xmin, c_float), real(xstep, c_float), real(xmax, c_float), &
                real(ymin, c_float), real(ymax, c_float))
  end subroutine gpc_ylog

  ! Vector arrow
  subroutine gpc_uvector(x1, y1, x2, y2, head, sz, opt)
    real, intent(in) :: x1, y1, x2, y2, head, sz
    character(len=*), intent(in), optional :: opt
    character(kind=c_char, len=:), allocatable, target :: c_opt

    if (present(opt)) then
      c_opt = trim(opt) // c_null_char
    else
      c_opt = "01" // c_null_char
    end if
    call c_uvector(real(x1, c_float), real(y1, c_float), real(x2, c_float), real(y2, c_float), &
                   real(head, c_float), real(sz, c_float), c_loc(c_opt))
  end subroutine gpc_uvector

  ! Turn off axes drawing
  subroutine gpc_axesoff()
    call c_axesoff()
  end subroutine gpc_axesoff

  ! Draw bounding box around ternary/polar plot
  subroutine gpc_pbox()
    call c_pbox()
  end subroutine gpc_pbox

  ! Set ternary component names
  subroutine gpc_aname(name)
    character(len=*), intent(in) :: name
    character(kind=c_char, len=:), allocatable, target :: c_n
    c_n = trim(name) // c_null_char
    call c_aname(c_loc(c_n))
  end subroutine gpc_aname

  subroutine gpc_bname(name)
    character(len=*), intent(in) :: name
    character(kind=c_char, len=:), allocatable, target :: c_n
    c_n = trim(name) // c_null_char
    call c_bname(c_loc(c_n))
  end subroutine gpc_bname

  subroutine gpc_cname(name)
    character(len=*), intent(in) :: name
    character(kind=c_char, len=:), allocatable, target :: c_n
    c_n = trim(name) // c_null_char
    call c_cname(c_loc(c_n))
  end subroutine gpc_cname

  subroutine gpc_ternary_names(a, b, c)
    character(len=*), intent(in) :: a, b, c
    call gpc_aname(a)
    call gpc_bname(b)
    call gpc_cname(c)
  end subroutine gpc_ternary_names

  ! Draw 2D grouped bar chart
  subroutine gpc_bar(x, y, npts, nxdiv, bwid, mode, colors, patterns)
    real, dimension(*), intent(in) :: x, y
    integer, intent(in) :: npts, nxdiv
    real, intent(in) :: bwid
    integer, intent(in) :: mode
    integer, dimension(*), intent(in), optional :: colors, patterns
    real(c_float), allocatable :: fx(:), fy(:)
    integer(c_int), allocatable, target :: iclr(:), ipat(:)
    type(c_ptr) :: p_clr, p_pat
    integer :: i

    allocate(fx(npts), fy(npts))
    do i = 1, npts
      fx(i) = real(x(i), c_float)
      fy(i) = real(y(i), c_float)
    end do

    p_clr = c_null_ptr
    if (present(colors)) then
      allocate(iclr(npts))
      do i = 1, npts
        iclr(i) = int(colors(i), c_int)
      end do
      p_clr = c_loc(iclr(1))
    end if

    p_pat = c_null_ptr
    if (present(patterns)) then
      allocate(ipat(npts))
      do i = 1, npts
        ipat(i) = int(patterns(i), c_int)
      end do
      p_pat = c_loc(ipat(1))
    end if

    call c_bar(int(nxdiv, c_int), fx, fy, int(npts, c_int), real(bwid, c_float), int(mode, c_int), p_clr, p_pat)

    deallocate(fx, fy)
    if (allocated(iclr)) deallocate(iclr)
    if (allocated(ipat)) deallocate(ipat)
  end subroutine gpc_bar

  ! High-level convenience alias for gpc_bar
  subroutine gpc_barchart(nbars, x, y, npts, width, carray, typearray)
    integer, intent(in) :: nbars, npts
    real, dimension(*), intent(in) :: x, y
    real, intent(in) :: width
    integer, dimension(*), intent(in), optional :: carray, typearray
    call gpc_bar(x, y, npts, nbars, width, 0, carray, typearray)
  end subroutine gpc_barchart

  ! Draw exploded pie chart
  subroutine gpc_pie(values, nslice, captions, colors, offsets, size)
    real, dimension(*), intent(in) :: values
    integer, intent(in) :: nslice
    character(len=*), dimension(*), intent(in), optional :: captions
    integer, dimension(*), intent(in), optional :: colors
    real, dimension(*), intent(in), optional :: offsets
    real, intent(in), optional :: size
    real(c_float), allocatable :: fval(:), foff(:)
    integer(c_int), allocatable :: iclr(:)
    type(c_ptr), allocatable, target :: c_caps(:)
    character(kind=c_char, len=128), allocatable, target :: str_buf(:)
    real(c_float) :: sz
    type(c_ptr) :: cap_ptr
    integer :: i
    integer(c_int) :: def_clrs(5) = [-4_c_int, -1_c_int, -2_c_int, -5_c_int, -3_c_int]

    allocate(fval(nslice), foff(nslice), iclr(nslice))
    do i = 1, nslice
      fval(i) = real(values(i), c_float)
      if (present(offsets)) then
        foff(i) = real(offsets(i), c_float)
      else
        foff(i) = 0.0_c_float
      end if
      if (present(colors)) then
        iclr(i) = -abs(int(colors(i), c_int))
      else
        iclr(i) = def_clrs(mod(i - 1, 5) + 1)
      end if
    end do

    sz = 2.0_c_float
    if (present(size)) sz = real(size, c_float)

    cap_ptr = c_null_ptr
    if (present(captions)) then
      allocate(c_caps(nslice), str_buf(nslice))
      do i = 1, nslice
        str_buf(i) = trim(captions(i)) // c_null_char
        c_caps(i) = c_loc(str_buf(i))
      end do
      cap_ptr = c_loc(c_caps(1))
    end if

    call c_pieplot(int(nslice, c_int), fval, iclr, foff, cap_ptr, sz, 14_c_int, 0_c_int)

    deallocate(fval, foff, iclr)
    if (allocated(c_caps)) deallocate(c_caps, str_buf)
  end subroutine gpc_pie

  ! Draw filled polygonal panel in inch coordinates
  subroutine gpc_panelinch(x, y, nlines, pattern, border)
    real, dimension(*), intent(in) :: x, y
    integer, intent(in) :: nlines, pattern
    integer, intent(in), optional :: border
    real(c_float), allocatable :: fx(:), fy(:)
    integer(c_int) :: brd
    integer :: i

    brd = 1_c_int
    if (present(border)) brd = int(border, c_int)

    allocate(fx(nlines), fy(nlines))
    do i = 1, nlines
      fx(i) = real(x(i), c_float)
      fy(i) = real(y(i), c_float)
    end do

    call c_panelinch(fx, fy, int(nlines, c_int), int(pattern, c_int), brd)
    call c_panproc()

    deallocate(fx, fy)
  end subroutine gpc_panelinch

  subroutine gpc_panproc()
    call c_panproc()
  end subroutine gpc_panproc

  ! Print font string in user plot coordinates
  subroutine gpc_pltfnt(x, y, str, height, angle)
    real, intent(in) :: x, y, height
    character(len=*), intent(in) :: str
    integer, intent(in), optional :: angle
    character(kind=c_char, len=:), allocatable, target :: c_s
    integer(c_int) :: ang

    ang = 0_c_int
    if (present(angle)) ang = int(angle, c_int)

    c_s = trim(str) // c_null_char
    call c_pltfnt(real(x, c_float), real(y, c_float), c_loc(c_s), real(height, c_float), ang)
  end subroutine gpc_pltfnt

  ! Print font string in page inch coordinates
  subroutine gpc_prtfnt(xinch, yinch, str, height, angle)
    real, intent(in) :: xinch, yinch, height
    character(len=*), intent(in) :: str
    integer, intent(in), optional :: angle
    character(kind=c_char, len=:), allocatable, target :: c_s
    integer(c_int) :: ang

    ang = 0_c_int
    if (present(angle)) ang = int(angle, c_int)

    c_s = trim(str) // c_null_char
    call c_prtfnt(real(xinch, c_float), real(yinch, c_float), c_loc(c_s), real(height, c_float), ang)
  end subroutine gpc_prtfnt

  ! Set 3D bar chart column width
  subroutine gpc_d3bars(bwid)
    real, intent(in) :: bwid
    call c_d3bars(real(bwid, c_float))
  end subroutine gpc_d3bars

  ! Simultaneous multiple plot contexts
  subroutine gpc_simuplot(numplots)
    integer, intent(in) :: numplots
    call c_simuplot(int(numplots, c_int))
  end subroutine gpc_simuplot

  ! Select active simultaneous plot context
  subroutine gpc_context(ctx)
    integer, intent(in) :: ctx
    call c_context(int(ctx, c_int))
  end subroutine gpc_context

  ! Crossed axes passing through (0, 0)
  subroutine gpc_cross(iscross)
    integer, intent(in) :: iscross
    character(kind=c_char) :: c_flag
    c_flag = char(iscross, kind=c_char)
    call c_cross(c_flag)
  end subroutine gpc_cross

  subroutine gpc_crossat(x, y)
    real, intent(in) :: x, y
    call c_CrossAt(real(x, c_float), real(y, c_float))
  end subroutine gpc_crossat

  subroutine gpc_frame(frameon, ftick)
    integer, intent(in) :: frameon, ftick
    character(kind=c_char) :: c_fon, c_ftk
    c_fon = char(frameon, kind=c_char)
    c_ftk = char(ftick, kind=c_char)
    call c_frame(c_fon, c_ftk)
  end subroutine gpc_frame

  subroutine gpc_tickht(ht)
    real, intent(in) :: ht
    call c_tickht(real(ht, c_float))
  end subroutine gpc_tickht

  subroutine gpc_tickout(is_out)
    integer, intent(in) :: is_out
    call c_tickout(int(is_out, c_int))
  end subroutine gpc_tickout

  subroutine gpc_numht(ht)
    real, intent(in) :: ht
    call c_numht(real(ht, c_float))
  end subroutine gpc_numht

  subroutine gpc_titlht(ht)
    real, intent(in) :: ht
    call c_titlht(real(ht, c_float))
  end subroutine gpc_titlht

  subroutine gpc_axnamht(ht)
    real, intent(in) :: ht
    call c_axnamht(real(ht, c_float))
  end subroutine gpc_axnamht

  subroutine gpc_upright(uplab)
    integer, intent(in) :: uplab
    character(kind=c_char) :: c_u
    c_u = char(uplab, kind=c_char)
    call c_upright(c_u)
  end subroutine gpc_upright

  subroutine gpc_acrop(flag)
    integer, intent(in) :: flag
    character(kind=c_char) :: c_f
    c_f = char(flag, kind=c_char)
    call c_acrop(c_f)
  end subroutine gpc_acrop

  subroutine gpc_rotate(isrot)
    integer, intent(in) :: isrot
    character(kind=c_char) :: c_r
    c_r = char(isrot, kind=c_char)
    call c_rotate(c_r)
  end subroutine gpc_rotate

  subroutine gpc_pgshift(x, y)
    real, intent(in) :: x, y
    call c_pgshift(real(x, c_float), real(y, c_float))
  end subroutine gpc_pgshift

  subroutine gpc_grid(kind)
    integer, intent(in) :: kind
    call c_grid(int(kind, c_int))
  end subroutine gpc_grid

  subroutine gpc_fgrid(fon, ndiv)
    integer, intent(in) :: fon, ndiv
    call c_fgrid(int(fon, c_int), int(ndiv, c_int))
  end subroutine gpc_fgrid

  subroutine gpc_xfgrid(fon, nx)
    integer, intent(in) :: fon, nx
    call c_xfgrid(int(fon, c_int), int(nx, c_int))
  end subroutine gpc_xfgrid

  subroutine gpc_yfgrid(fon, ny)
    integer, intent(in) :: fon, ny
    call c_yfgrid(int(fon, c_int), int(ny, c_int))
  end subroutine gpc_yfgrid

  subroutine gpc_box3d()
    call c_box3d()
  end subroutine gpc_box3d

  subroutine gpc_d3line(xf, yf, zf, xt, yt, zt)
    real, intent(in) :: xf, yf, zf, xt, yt, zt
    call c_d3line(real(xf, c_float), real(yf, c_float), real(zf, c_float), &
                  real(xt, c_float), real(yt, c_float), real(zt, c_float))
  end subroutine gpc_d3line

  subroutine gpc_curv3d(x, y, z, n)
    real(c_float), dimension(*), intent(in) :: x, y, z
    integer, intent(in) :: n
    call c_curv3d(x, y, z, int(n, c_int))
  end subroutine gpc_curv3d

  subroutine gpc_d3color(colors, nlevels, mesh)
    integer(c_int), dimension(*), intent(in) :: colors
    integer, intent(in) :: nlevels, mesh
    call c_d3color(colors, int(nlevels, c_int), char(mesh, kind=c_char))
  end subroutine gpc_d3color

  subroutine gpc_bcolor(val)
    integer, intent(in) :: val
    call c_bcolor(char(val, kind=c_char))
  end subroutine gpc_bcolor

  subroutine gpc_tcolor(val)
    integer, intent(in) :: val
    call c_tcolor(char(val, kind=c_char))
  end subroutine gpc_tcolor

  subroutine gpc_d3head(title, place)
    character(len=*), intent(in) :: title
    integer, intent(in), optional :: place
    character(kind=c_char, len=:), allocatable, target :: c_t
    integer(c_int) :: pl
    pl = 0_c_int
    if (present(place)) pl = int(place, c_int)
    c_t = trim(title) // c_null_char
    call c_d3head(c_loc(c_t), pl)
  end subroutine gpc_d3head

  subroutine gpc_x3name(name)
    character(len=*), intent(in) :: name
    character(kind=c_char, len=:), allocatable, target :: c_n
    c_n = trim(name) // c_null_char
    call c_x3name(c_loc(c_n))
  end subroutine gpc_x3name

  subroutine gpc_y3name(name)
    character(len=*), intent(in) :: name
    character(kind=c_char, len=:), allocatable, target :: c_n
    c_n = trim(name) // c_null_char
    call c_y3name(c_loc(c_n))
  end subroutine gpc_y3name

  subroutine gpc_z3name(name)
    character(len=*), intent(in) :: name
    character(kind=c_char, len=:), allocatable, target :: c_n
    c_n = trim(name) // c_null_char
    call c_z3name(c_loc(c_n))
  end subroutine gpc_z3name

  subroutine gpc_sclfix(isfixed)
    integer, intent(in) :: isfixed
    call c_sclfix(int(isfixed, c_int))
  end subroutine gpc_sclfix

  subroutine gpc_nohide(visible)
    integer, intent(in) :: visible
    call c_nohide(int(visible, c_int))
  end subroutine gpc_nohide

  subroutine gpc_vuangl(phi, theta, radius)
    real, intent(in) :: phi, theta, radius
    call c_vuangl(real(phi, c_float), real(theta, c_float), real(radius, c_float))
  end subroutine gpc_vuangl

  subroutine gpc_contcolor(colors, border)
    integer(c_int), dimension(*), intent(in) :: colors
    integer, intent(in) :: border
    call c_contcolor_fixed(colors, int(border, c_int), c_null_ptr)
  end subroutine gpc_contcolor

  subroutine gpc_curvfill(x, y, npts, x1, y1, npts1, pcolor, border)
    real(c_float), dimension(*), intent(in) :: x, y, x1, y1
    integer, intent(in) :: npts, npts1, pcolor, border
    call c_curvfill(x, y, int(npts, c_int), x1, y1, int(npts1, c_int), &
                    int(pcolor, c_int), int(border, c_int))
  end subroutine gpc_curvfill

  subroutine gpc_errorbar(x, xmin, xmax, y, ymin, ymax)
    real, intent(in) :: x, xmin, xmax, y, ymin, ymax
    call c_errorbar(real(x, c_float), real(xmin, c_float), real(xmax, c_float), &
                    real(y, c_float), real(ymin, c_float), real(ymax, c_float))
  end subroutine gpc_errorbar

  subroutine gpc_scatplot(dot)
    integer, intent(in) :: dot
    call c_scatplot(char(dot, kind=c_char))
  end subroutine gpc_scatplot

  subroutine gpc_histogram(x, y, npts, type_val, colors)
    real(c_float), dimension(*), intent(in) :: x, y
    integer, intent(in) :: npts, type_val
    integer(c_int), dimension(*), intent(in), target :: colors
    call c_histogram(x, y, int(npts, c_int), int(type_val, c_int), c_loc(colors))
  end subroutine gpc_histogram

  subroutine gpc_ctline(str, height)
    character(len=*), intent(in) :: str
    real, intent(in) :: height
    character(kind=c_char, len=:), allocatable, target :: c_s
    c_s = trim(str) // c_null_char
    call c_ctline(c_loc(c_s), real(height, c_float))
  end subroutine gpc_ctline

  subroutine gpc_ltline(str, height)
    character(len=*), intent(in) :: str
    real, intent(in) :: height
    character(kind=c_char, len=:), allocatable, target :: c_s
    c_s = trim(str) // c_null_char
    call c_ltline(c_loc(c_s), real(height, c_float))
  end subroutine gpc_ltline

  subroutine gpc_rtline(str, height)
    character(len=*), intent(in) :: str
    real, intent(in) :: height
    character(kind=c_char, len=:), allocatable, target :: c_s
    c_s = trim(str) // c_null_char
    call c_rtline(c_loc(c_s), real(height, c_float))
  end subroutine gpc_rtline

  function gpc_strwidth(str, height) result(w)
    character(len=*), intent(in) :: str
    real, intent(in) :: height
    real :: w
    character(kind=c_char, len=:), allocatable, target :: c_s
    c_s = trim(str) // c_null_char
    w = c_strwidth(c_loc(c_s), real(height, c_float))
  end function gpc_strwidth

  subroutine gpc_subsym(ch)
    character, intent(in) :: ch
    call c_subsym(ch)
  end subroutine gpc_subsym

  subroutine gpc_legend(marker, str, style, strhgt, scolor)
    integer, intent(in) :: marker, style, scolor
    character(len=*), intent(in) :: str
    real, intent(in) :: strhgt
    character(kind=c_char, len=:), allocatable, target :: c_s
    c_s = trim(str) // c_null_char
    call c_legend(int(marker, c_int), c_loc(c_s), int(style, c_int), &
                  real(strhgt, c_float), int(scolor, c_int))
  end subroutine gpc_legend

  subroutine gpc_legpos(entries, xleg, yleg, border)
    integer, intent(in) :: entries, border
    real, intent(in) :: xleg, yleg
    call c_legpos(int(entries, c_int), real(xleg, c_float), real(yleg, c_float), int(border, c_int))
  end subroutine gpc_legpos

  subroutine gpc_scales(nxdiv, nydiv, x, y, npts)
    integer, intent(in) :: nxdiv, nydiv, npts
    real(c_float), dimension(*), intent(inout) :: x, y
    call c_scales(int(nxdiv, c_int), int(nydiv, c_int), x, y, int(npts, c_int))
  end subroutine gpc_scales

  subroutine gpc_lglgscle(x, y, npts)
    real(c_float), dimension(*), intent(inout) :: x, y
    integer, intent(in) :: npts
    call c_lglgscle(x, y, int(npts, c_int))
  end subroutine gpc_lglgscle

  subroutine gpc_loglog(xorig, xmax, yorig, ymax)
    real, intent(in) :: xorig, xmax, yorig, ymax
    call c_loglog(real(xorig, c_float), real(xmax, c_float), &
                  real(yorig, c_float), real(ymax, c_float))
  end subroutine gpc_loglog

  subroutine gpc_wedge(rmin, rmax, thmin, thmax, pattern, border)
    real, intent(in) :: rmin, rmax, thmin, thmax
    integer, intent(in) :: pattern, border
    call c_wedge(real(rmin, c_float), real(rmax, c_float), &
                 real(thmin, c_float), real(thmax, c_float), &
                 int(pattern, c_int), int(border, c_int))
  end subroutine gpc_wedge

  subroutine gpc_stack(stacked)
    integer, intent(in) :: stacked
    call c_stack(char(stacked, kind=c_char))
  end subroutine gpc_stack

  subroutine gpc_plotrho(rho, theta)
    real, intent(in) :: rho, theta
    call c_plotrho(real(rho, c_float), real(theta, c_float))
  end subroutine gpc_plotrho

  subroutine gpc_smithcir(u, v, radius)
    real, intent(in) :: u, v, radius
    call c_smithcir(real(u, c_float), real(v, c_float), real(radius, c_float))
  end subroutine gpc_smithcir

  subroutine gpc_smove(dell)
    real, intent(in) :: dell
    call c_smove(real(dell, c_float))
  end subroutine gpc_smove

  subroutine gpc_tline(width)
    integer, intent(in) :: width
    call c_tline(int(width, c_int))
  end subroutine gpc_tline

  subroutine gpc_tcurve(width)
    integer, intent(in) :: width
    call c_tcurve(int(width, c_int))
  end subroutine gpc_tcurve

  subroutine gpc_plotadd()
    call c_plotadd()
  end subroutine gpc_plotadd

  subroutine gpc_plotabsolute(flag)
    integer, intent(in) :: flag
    call c_plotabsolute(int(flag, c_int))
  end subroutine gpc_plotabsolute

  subroutine gpc_NewFile(name)
    character(len=*), intent(in) :: name
    character(kind=c_char, len=:), allocatable, target :: c_n
    c_n = trim(name) // c_null_char
    call c_NewFile(c_loc(c_n))
  end subroutine gpc_NewFile

  subroutine gpc_dateit(font)
    character, intent(in) :: font
    call c_dateit(font)
  end subroutine gpc_dateit

  subroutine gpc_linesp(ratio)
    real, intent(in) :: ratio
    call c_linesp(real(ratio, c_float))
  end subroutine gpc_linesp

  subroutine gpc_lmargin(marginch)
    real, intent(in) :: marginch
    call c_lmargin(real(marginch, c_float))
  end subroutine gpc_lmargin

  subroutine gpc_rmargin(marginch)
    real, intent(in) :: marginch
    call c_rmargin(real(marginch, c_float))
  end subroutine gpc_rmargin

  subroutine gpc_tmargin(marginch)
    real, intent(in) :: marginch
    call c_tmargin(real(marginch, c_float))
  end subroutine gpc_tmargin

  subroutine gpc_charspc(width)
    integer, intent(in) :: width
    call c_charspc(int(width, c_int))
  end subroutine gpc_charspc

  subroutine gpc_skew(slant)
    real, intent(in) :: slant
    call c_skew(real(slant, c_float))
  end subroutine gpc_skew

  subroutine gpc_widen(fact)
    real, intent(in) :: fact
    call c_widen(real(fact, c_float))
  end subroutine gpc_widen

  subroutine gpc_tfont(wid)
    integer, intent(in) :: wid
    call c_tfont(int(wid, c_int))
  end subroutine gpc_tfont

  subroutine gpc_tifont(wid)
    real, intent(in) :: wid
    call c_tifont(real(wid, c_float))
  end subroutine gpc_tifont

  subroutine gpc_fntchg(sym)
    character, intent(in) :: sym
    call c_fntchg(sym)
  end subroutine gpc_fntchg

  subroutine gpc_supsym(ch)
    character, intent(in) :: ch
    call c_supsym(ch)
  end subroutine gpc_supsym

  subroutine gpc_putlabel(str, xpos, ypos, height, border, units)
    character(len=*), intent(in) :: str
    real, intent(in) :: xpos, ypos, height
    integer, intent(in) :: border, units
    character(kind=c_char, len=:), allocatable, target :: c_s
    c_s = trim(str) // c_null_char
    call c_putlabel(c_loc(c_s), real(xpos, c_float), real(ypos, c_float), &
                    real(height, c_float), int(border, c_int), int(units, c_int))
  end subroutine gpc_putlabel

  subroutine gpc_d3base(isbase, colorb)
    integer, intent(in) :: isbase, colorb
    call c_d3base(int(isbase, c_int), int(colorb, c_int))
  end subroutine gpc_d3base

  subroutine gpc_fillfont(fill)
    integer, intent(in) :: fill
    call c_fillfont(int(fill, c_int))
  end subroutine gpc_fillfont

  subroutine gpc_fontfill(fill, pattern, border)
    integer, intent(in) :: fill, pattern, border
    call c_fontfill(int(fill, c_int), int(pattern, c_int), int(border, c_int))
  end subroutine gpc_fontfill


  ! --- Full Parity Additions for Fortran ---

  subroutine gpc_boxplot(x, y, outlier, symnum, width)
    real, intent(in) :: x, width
    real(c_float), target, intent(in) :: y(:)
    integer, intent(in) :: outlier, symnum
    call c_boxplot(real(x, c_float), c_loc(y), int(size(y), c_int), &
                   int(outlier, c_int), int(symnum, c_int), real(width, c_float))
  end subroutine gpc_boxplot

  subroutine gpc_plane3d(wplane, pos, hide)
    character, intent(in) :: wplane, hide
    real, intent(in) :: pos
    call c_plane3d(wplane, real(pos, c_float), hide)
  end subroutine gpc_plane3d

  subroutine gpc_waterfallcurve(x, y, z, npts)
    real(c_float), target, intent(in) :: x(*)
    real, intent(in) :: y
    real(c_float), target, intent(in) :: z(*)
    integer, intent(in) :: npts
    call c_waterfallcurve(c_loc(x), real(y, c_float), c_loc(z), int(npts, c_int))
  end subroutine gpc_waterfallcurve

  subroutine gpc_waterfallplot(x, y, z, nx, ny)
    real(c_float), target, intent(in) :: x(*)
    real(c_float), target, intent(in) :: y(*)
    real(c_float), target, intent(in) :: z(nx, ny)
    integer, intent(in) :: nx, ny
    type(c_ptr), allocatable, target :: row_ptrs(:)
    integer :: i

    allocate(row_ptrs(ny))
    do i = 1, ny
      row_ptrs(i) = c_loc(z(1, i))
    end do
    call c_waterfallplot(c_loc(x), c_loc(y), c_loc(row_ptrs), int(nx, c_int), int(ny, c_int))
    deallocate(row_ptrs)
  end subroutine gpc_waterfallplot

  subroutine gpc_colorkey(minv, maxv, intensity, legllx, leglly, legurx, legury, labint, format_str, fontc, height, angle)
    real, intent(in) :: minv, maxv, legllx, leglly, legurx, legury, height
    integer(c_int), target, intent(in) :: intensity(:)
    integer, intent(in) :: labint, angle
    character(len=*), intent(in) :: format_str
    character, intent(in) :: fontc
    character(kind=c_char, len=:), allocatable, target :: c_fmt
    c_fmt = trim(format_str) // c_null_char
    call c_colorkeyl(real(minv, c_float), real(maxv, c_float), c_loc(intensity), &
                     int(size(intensity), c_int), real(legllx, c_float), real(leglly, c_float), &
                     real(legurx, c_float), real(legury, c_float), int(labint, c_int), &
                     c_loc(c_fmt), fontc, real(height, c_float), int(angle, c_int))
  end subroutine gpc_colorkey

  subroutine gpc_erfcurve(x, y, sym)
    real(c_float), target, intent(in) :: x(:), y(:)
    integer, intent(in) :: sym
    integer :: n
    n = min(size(x), size(y))
    call c_erfcurve(c_loc(x), c_loc(y), int(n, c_int), int(sym, c_int))
  end subroutine gpc_erfcurve

  subroutine gpc_erfgrid(ymin, ystep, ymax, yformat)
    real, intent(in) :: ymin, ystep, ymax
    character(len=*), intent(in) :: yformat
    character(kind=c_char, len=:), allocatable, target :: c_fmt
    c_fmt = trim(yformat) // c_null_char
    call c_erfgrid(real(ymin, c_float), real(ystep, c_float), real(ymax, c_float), c_loc(c_fmt))
  end subroutine gpc_erfgrid

  subroutine gpc_erfscales(nydiv, y)
    integer, intent(in) :: nydiv
    real(c_float), target, intent(in) :: y(:)
    call c_erfscales(int(nydiv, c_int), c_loc(y), int(size(y), c_int))
  end subroutine gpc_erfscales

  subroutine gpc_usertoinch(x, y, x_out, y_out)
    real, intent(in) :: x, y
    real, intent(out) :: x_out, y_out
    real(c_float), target :: xo, yo
    call c_usertoinch(real(x, c_float), real(y, c_float), c_loc(xo), c_loc(yo))
    x_out = real(xo)
    y_out = real(yo)
  end subroutine gpc_usertoinch

  subroutine gpc_usertopix(x, y, z, x_out, y_out)
    real, intent(in) :: x, y, z
    integer, intent(out) :: x_out, y_out
    integer(c_int), target :: xo, yo
    call c_usertopix(real(x, c_float), real(y, c_float), real(z, c_float), c_loc(xo), c_loc(yo))
    x_out = int(xo)
    y_out = int(yo)
  end subroutine gpc_usertopix

  function gpc_intopix(inches) result(pixels)
    real, intent(in) :: inches
    integer :: pixels
    pixels = int(c_intopix(real(inches, c_float)))
  end function gpc_intopix

  function gpc_pixtoin(pixels) result(inches)
    integer, intent(in) :: pixels
    real :: inches
    inches = real(c_pixtoin(int(pixels, c_int)))
  end function gpc_pixtoin

  subroutine gpc_ellipse(xc, yc, xr, yr, rotation, th1, th2, degrees, units)
    real, intent(in) :: xc, yc, xr, yr, rotation, th1, th2
    integer, intent(in) :: degrees, units
    call c_ellipse(real(xc, c_float), real(yc, c_float), real(xr, c_float), real(yr, c_float), &
                   real(rotation, c_float), real(th1, c_float), real(th2, c_float), &
                   char(degrees), int(units, c_int))
  end subroutine gpc_ellipse

  subroutine gpc_fillellipse(onoff, pattern, border)
    integer, intent(in) :: onoff, pattern, border
    call c_fillellipse(int(onoff, c_int), int(pattern, c_int), int(border, c_int))
  end subroutine gpc_fillellipse

  subroutine gpc_blank(attr)
    integer, intent(in) :: attr
    call c_blank(int(attr, c_int))
  end subroutine gpc_blank

  subroutine gpc_d3grid(subdiv, label)
    integer, intent(in) :: subdiv, label
    call c_d3grid(int(subdiv, c_int), int(label, c_int))
  end subroutine gpc_d3grid

  subroutine gpc_d3pltfnt(x, y, z, str, height, angle)
    real, intent(in) :: x, y, z, height
    character(len=*), intent(in) :: str
    integer, intent(in) :: angle
    character(kind=c_char, len=:), allocatable, target :: c_s
    c_s = trim(str) // c_null_char
    call c_d3pltfnt(real(x, c_float), real(y, c_float), real(z, c_float), &
                    c_loc(c_s), real(height, c_float), int(angle, c_int))
  end subroutine gpc_d3pltfnt

  subroutine gpc_hlstorgb(h, l, s, r, g, b)
    integer, intent(in) :: h, l, s
    integer, intent(out) :: r, g, b
    integer(c_int), target :: ro, go, bo
    call c_hlstorgb(int(h, c_int), int(l, c_int), int(s, c_int), c_loc(ro), c_loc(go), c_loc(bo))
    r = int(ro)
    g = int(go)
    b = int(bo)
  end subroutine gpc_hlstorgb

  subroutine gpc_rgbtohls(r, g, b, h, l, s)
    integer, intent(in) :: r, g, b
    integer, intent(out) :: h, l, s
    integer(c_int), target :: ho, lo, so
    call c_rgbtohls(int(r, c_int), int(g, c_int), int(b, c_int), c_loc(ho), c_loc(lo), c_loc(so))
    h = int(ho)
    l = int(lo)
    s = int(so)
  end subroutine gpc_rgbtohls

  function gpc_mean(x) result(res)
    real(c_float), target, intent(in) :: x(:)
    real :: res
    res = real(c_mean(c_loc(x), int(size(x), c_int)))
  end function gpc_mean

  function gpc_median(x) result(res)
    real(c_float), target, intent(in) :: x(:)
    real :: res
    res = real(c_median(c_loc(x), int(size(x), c_int)))
  end function gpc_median

  function gpc_stddev(x) result(res)
    real(c_float), target, intent(in) :: x(:)
    real :: res
    res = real(c_stddev(c_loc(x), int(size(x), c_int)))
  end function gpc_stddev

  subroutine gpc_minmax(xy, minq, maxq)
    real(c_float), target, intent(in) :: xy(:)
    real, intent(out) :: minq, maxq
    real(c_float), target :: mi, ma
    call c_minmax(c_loc(mi), c_loc(ma), c_loc(xy), int(size(xy), c_int))
    minq = real(mi)
    maxq = real(ma)
  end subroutine gpc_minmax

  subroutine gpc_setttfull(onoff, str)
    integer, intent(in) :: onoff, str
    call c_setttfull(char(onoff), char(str))
  end subroutine gpc_setttfull

  subroutine gpc_setttkerning(onoff, str)
    integer, intent(in) :: onoff, str
    call c_setttkerning(char(onoff), char(str))
  end subroutine gpc_setttkerning

  subroutine gpc_setttmaxerror(err, str)
    real, intent(in) :: err
    integer, intent(in) :: str
    call c_setttmaxerror(real(err, c_float), char(str))
  end subroutine gpc_setttmaxerror

  subroutine gpc_setttusememory(onoff)
    integer, intent(in) :: onoff
    call c_setttusememory(char(onoff))
  end subroutine gpc_setttusememory

  subroutine gpc_settolerance(tol)
    real, intent(in) :: tol
    call c_settolerance(real(tol, c_float))
  end subroutine gpc_settolerance

  subroutine gpc_saveplotq()
    call c_saveplotq()
  end subroutine gpc_saveplotq


  subroutine gpc_pixtouser(x_in, y_in, x_out, y_out)
    integer, intent(in) :: x_in, y_in
    real, intent(out) :: x_out, y_out
    real(c_float), target :: xo, yo
    call c_pixtouser(int(x_in, c_int), int(y_in, c_int), c_loc(xo), c_loc(yo))
    x_out = real(xo)
    y_out = real(yo)
  end subroutine gpc_pixtouser

  subroutine gpc_beep(freq, duration)
    integer, intent(in) :: freq, duration
    call c_gpcbeep(int(freq, c_int), int(duration, c_int))
  end subroutine gpc_beep

  subroutine gpc_set_time(timer_id)
    integer, intent(in) :: timer_id
    call c_set_time(int(timer_id, c_int))
  end subroutine gpc_set_time

  subroutine gpc_trimat(m1, m2, z, dim1, zmin, zstep, zmax, lnstyle, border)
    real(kind=c_float), dimension(:), target, intent(in) :: m1, m2, z
    integer, intent(in) :: dim1
    real, intent(in) :: zmin, zstep, zmax
    integer(kind=c_int), dimension(:), target, intent(in) :: lnstyle
    integer, intent(in) :: border
    integer :: dim

    dim = min(size(m1), size(m2), size(z))
    call c_trimat(c_loc(m1(1)), c_loc(m2(1)), c_loc(z(1)), int(dim, c_int), int(dim1, c_int), &
                  real(zmin, c_float), real(zstep, c_float), real(zmax, c_float), &
                  c_loc(lnstyle(1)), int(border, c_int))
  end subroutine gpc_trimat

  subroutine gpc_trifun(fun, dim, zmin, zstep, zmax, lnstyle, border)
    procedure(gpc_funq) :: fun
    integer, intent(in) :: dim
    real, intent(in) :: zmin, zstep, zmax
    integer(kind=c_int), dimension(:), target, intent(in) :: lnstyle
    integer, intent(in) :: border

    call c_trifun(c_funloc(fun), int(dim, c_int), real(zmin, c_float), &
                  real(zstep, c_float), real(zmax, c_float), &
                  c_loc(lnstyle(1)), int(border, c_int))
  end subroutine gpc_trifun

  subroutine gpc_surmat2(zmat, nx, ny)
    real(kind=c_float), dimension(nx, ny), target, intent(in) :: zmat
    integer, intent(in) :: nx, ny
    type(c_ptr), dimension(ny), target :: row_ptrs
    integer :: j

    do j = 1, ny
      row_ptrs(j) = c_loc(zmat(1, j))
    end do
    call c_surmat2(c_loc(row_ptrs(1)), int(nx, c_int), int(ny, c_int))
  end subroutine gpc_surmat2

  subroutine gpc_surfun(fun, ixpts, xdel, iypts, ydel)
    procedure(gpc_funq) :: fun
    integer, intent(in) :: ixpts, iypts
    real, intent(in) :: xdel, ydel

    call c_surfun(c_funloc(fun), int(ixpts, c_int), real(xdel, c_float), &
                  int(iypts, c_int), real(ydel, c_float))
  end subroutine gpc_surfun

  subroutine gpc_spline(x, y, n1, s1, s2, xn, yn)
    real(kind=c_float), dimension(:), target, intent(in) :: x, y
    integer, intent(in) :: n1
    real, intent(in) :: s1, s2
    real(kind=c_float), dimension(n1), target, intent(out) :: xn, yn
    integer :: n

    n = min(size(x), size(y))
    call c_spline(c_loc(x(1)), c_loc(y(1)), int(n, c_int), int(n1, c_int), &
                  real(s1, c_float), real(s2, c_float), &
                  c_loc(xn(1)), c_loc(yn(1)))
  end subroutine gpc_spline

  ! Pack 8-bit RGB into GraphiC 24-bit RGB integer
  function gpc_rgb(r, g, b) result(res)
    integer, intent(in) :: r, g, b
    integer :: res
    res = int(c_gpc_rgb(int(r, c_int), int(g, c_int), int(b, c_int)))
  end function gpc_rgb

  ! Evaluate colormap at normalized position t in [0.0, 1.0]
  function gpc_colormap_eval(cmap_id, t) result(res)
    integer, intent(in) :: cmap_id
    real, intent(in) :: t
    integer :: res
    res = int(c_gpc_colormap_eval(int(cmap_id, c_int), real(t, c_float)))
  end function gpc_colormap_eval

  ! Query colormap at a specific scalar value between minv and maxv -> returns color int
  function gpc_get_color(cmap_id, val, minv, maxv) result(res)
    integer, intent(in) :: cmap_id
    real, intent(in) :: val, minv, maxv
    integer :: res
    res = int(c_gpc_get_color(int(cmap_id, c_int), real(val, c_float), real(minv, c_float), real(maxv, c_float)))
  end function gpc_get_color

  ! Query colormap RGB components at a specific scalar value
  subroutine gpc_get_rgb(cmap_id, val, minv, maxv, r, g, b)
    integer, intent(in) :: cmap_id
    real, intent(in) :: val, minv, maxv
    integer, intent(out) :: r, g, b
    integer(c_int), target :: cr, cg, cb
    call c_gpc_get_rgb(int(cmap_id, c_int), real(val, c_float), real(minv, c_float), real(maxv, c_float), &
                       c_loc(cr), c_loc(cg), c_loc(cb))
    r = int(cr)
    g = int(cg)
    b = int(cb)
  end subroutine gpc_get_rgb

  ! Continuous scatter plot routine
  subroutine gpc_scatter(x, y, z, minv, maxv, symnum, symheight, cmap_id)
    real(c_float), dimension(:), target, intent(in) :: x, y, z
    real, intent(in) :: minv, maxv, symheight
    integer, intent(in) :: symnum, cmap_id
    integer :: n
    n = min(size(x), min(size(y), size(z)))
    call c_gpc_scatter(c_loc(x(1)), c_loc(y(1)), c_loc(z(1)), int(n, c_int), &
                       real(minv, c_float), real(maxv, c_float), &
                       int(symnum, c_int), real(symheight, c_float), int(cmap_id, c_int))
  end subroutine gpc_scatter

  ! Continuous gradient color key / color bar
  subroutine gpc_colorkey_gradient(minv, maxv, cmap_id, legllx, leglly, legurx, legury, nticks, format_str, fontc, height, angle)
    real, intent(in) :: minv, maxv, legllx, leglly, legurx, legury, height
    integer, intent(in) :: cmap_id, nticks, angle
    character(len=*), intent(in) :: format_str
    character, intent(in) :: fontc
    character(kind=c_char, len=:), allocatable, target :: c_fmt
    c_fmt = trim(format_str) // c_null_char
    call c_colorkey_gradient(real(minv, c_float), real(maxv, c_float), int(cmap_id, c_int), &
                             real(legllx, c_float), real(leglly, c_float), &
                             real(legurx, c_float), real(legury, c_float), &
                             int(nticks, c_int), c_loc(c_fmt), fontc, &
                             real(height, c_float), int(angle, c_int))
  end subroutine gpc_colorkey_gradient

  ! Load a font by name with escape character
  subroutine gpc_font(fontname, esc_char)
    character(len=*), intent(in) :: fontname
    character, intent(in) :: esc_char
    character(kind=c_char, len=:), allocatable, target :: c_fname
    c_fname = trim(fontname) // c_null_char
    call c_gpc_load_font(c_loc(c_fname), esc_char)
  end subroutine gpc_font

end module graphic
