!===============================================================================
! GraphiC Fortran 2003 Suite: d2test
! 2D Multi-Curve Harmonic Oscillations with Dual Line Styles
!
! How to Compile & Run (from GraphiC repository root):
!   make fortran_examples
!   ./fortran/bin/d2test
!
! Direct Command-Line Compilation:
!   gfortran -O2 -I fortran/bin -J fortran/bin d2test.f90 \
!       fortran/bin/graphic_iso.o fortran/bin/graphic.o lib/libgraphic.a -lm -o fortran/bin/d2test
!   ./fortran/bin/d2test
!
! Output:
!   d2test.svg (Vector SVG plot)
!===============================================================================

program d2test
  use graphic
  implicit none

  integer, parameter :: NPTS = 50
  real :: x1(NPTS), y1(NPTS), x2(NPTS), y2(NPTS)
  real :: t
  integer :: i

  do i = 1, NPTS
    t = real(i - 1) * 10.0 / real(NPTS - 1)
    x1(i) = t
    y1(i) = sin(t) * exp(-0.1 * t)
    x2(i) = t
    y2(i) = cos(t) * exp(-0.1 * t)
  end do

  call gpc_begin(DRIVER_SVG, "d2test.svg")
  call gpc_page(8.0, 6.0)
  call gpc_start(COLOR_WHITE)
  call gpc_area2d(5.5, 4.0)
  call gpc_physor(1.5, 1.2)
  call gpc_box()
  call gpc_title("Fortran 2003 Harmonic Oscillations", "Time (seconds)", "Amplitude (V)")
  call gpc_graf("%3.1f", 0.0, 1.0, 10.0, "%3.1f", -1.0, 0.5, 1.0)
  
  call gpc_color(COLOR_BLUE)
  call gpc_curve(x1, y1, NPTS, 0)
  
  call gpc_color(COLOR_RED)
  call gpc_curve(x2, y2, NPTS, 1)

  call gpc_end()
  call gpc_stop()
  print *, "Generated d2test.svg successfully"
end program d2test
