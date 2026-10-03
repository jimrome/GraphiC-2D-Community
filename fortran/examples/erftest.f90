!===============================================================================
! GraphiC Fortran 2003 Suite: erftest
! Error Functions and Gaussian Normal Probability Distributions
!
! How to Compile & Run (from GraphiC repository root):
!   make fortran_examples
!   ./fortran/bin/erftest
!
! Direct Command-Line Compilation:
!   gfortran -O2 -I fortran/bin -J fortran/bin erftest.f90 \
!       fortran/bin/graphic_iso.o fortran/bin/graphic.o lib/libgraphic.a -lm -o fortran/bin/erftest
!   ./fortran/bin/erftest
!
! Output:
!   erftest.svg (Vector SVG plot)
!===============================================================================

program erftest
  use graphic
  implicit none

  integer, parameter :: N = 100
  real :: x(N), y_erf(N), y_gauss(N)
  real :: v, pi
  integer :: i

  pi = 4.0 * atan(1.0)
  do i = 1, N
    v = -3.0 + real(i - 1) * 0.06
    x(i) = v
    y_erf(i) = erf(v)
    y_gauss(i) = exp(-v * v) / sqrt(pi)
  end do

  call gpc_begin(DRIVER_SVG, "erftest.svg")
  call gpc_page(8.0, 6.0)
  call gpc_start(COLOR_WHITE)
  call gpc_area2d(5.5, 4.0)
  call gpc_physor(1.5, 1.2)
  call gpc_box()
  call gpc_title("Error Function and Gaussian Distribution (Fortran Erftest)", "x", "Probability")
  call gpc_graf("%3.1f", -3.0, 1.0, 3.0, "%3.1f", -1.0, 0.5, 1.0)

  call gpc_color(COLOR_BLUE)
  call gpc_curve(x, y_erf, N, 0)

  call gpc_color(COLOR_GREEN)
  call gpc_curve(x, y_gauss, N, 0)

  call gpc_end()
  call gpc_stop()
  print *, "Generated erftest.svg successfully"
end program erftest
