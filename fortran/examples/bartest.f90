!===============================================================================
! GraphiC Fortran 2003 Suite: bartest
! 2D Grouped Bar Chart with Multiple Shading Patterns & Legend
!
! How to Compile & Run (from GraphiC repository root):
!   make fortran_examples
!   ./fortran/bin/bartest
!
! Direct Command-Line Compilation:
!   gfortran -O2 -I fortran/bin -J fortran/bin bartest.f90 \
!       fortran/bin/graphic_iso.o fortran/bin/graphic.o lib/libgraphic.a -lm -o fortran/bin/bartest
!   ./fortran/bin/bartest
!
! Output:
!   bartest.svg (Vector SVG plot)
!===============================================================================

program bartest
  use graphic
  implicit none

  integer, parameter :: NB = 4
  real :: x(NB) = [1.0, 2.0, 3.0, 4.0]
  real :: y(NB) = [12.5, 18.2, 15.0, 22.8]
  integer :: colors(NB) = [COLOR_BLUE, COLOR_GREEN, COLOR_RED, COLOR_BLACK]
  integer :: patterns(NB) = [1, 2, 5, 6]

  call gpc_begin(DRIVER_SVG, "bartest.svg")
  call gpc_page(8.0, 6.0)
  call gpc_start(COLOR_WHITE)
  call gpc_area2d(5.5, 4.0)
  call gpc_physor(1.5, 1.2)
  call gpc_box()
  call gpc_title("Quarterly Revenue by Division (Fortran Bartest)", "Quarter", "Revenue ($ Millions)")
  call gpc_graf("%1.0f", 0.0, 1.0, 5.0, "%3.1f", 0.0, 5.0, 25.0)

  call gpc_bar(x, y, NB, 5, 0.6, 0, colors, patterns)

  call gpc_end()
  call gpc_stop()
  print *, "Generated bartest.svg successfully"
end program bartest
