!===============================================================================
! GraphiC Fortran 2003 Suite: multi_y
! Multiple Independent Scaled Y-Axes with Distinct Line Styles
!
! How to Compile & Run (from GraphiC repository root):
!   make fortran_examples
!   ./fortran/bin/multi_y
!
! Direct Command-Line Compilation:
!   gfortran -O2 -I fortran/bin -J fortran/bin multi_y.f90 \
!       fortran/bin/graphic_iso.o fortran/bin/graphic.o lib/libgraphic.a -lm -o fortran/bin/multi_y
!   ./fortran/bin/multi_y
!
! Output:
!   multi_y.svg (Vector SVG plot)
!===============================================================================

program multi_y
  use graphic
  implicit none

  call gpc_begin(DRIVER_SVG, "multi_y.svg")
  call gpc_page(9.0, 7.0)
  call gpc_start(COLOR_WHITE)

  call gpc_simuplot(4)

  ! Context 0
  call gpc_context(0)
  call gpc_physor(1.5, 0.8)
  call gpc_area2d(6.0, 5.0)
  call gpc_color(COLOR_BLUE)
  call gpc_graf("%-4.1f", 0.0, 1.0, 10.0, "%-4.1f", 0.0, 2.0, 12.0)
  call gpc_pltfnt(2.0, 6.0, "Context 1 (Blue Scale)", 0.15, 0)

  ! Context 1
  call gpc_context(1)
  call gpc_physor(1.5, 0.8)
  call gpc_orel(-0.35, 0.0)
  call gpc_area2d(6.0, 5.0)
  call gpc_color(COLOR_GREEN)
  call gpc_graf("%-4.1f", 0.0, 1.0, 10.0, "%-4.1f", 0.0, 1.0, 6.0)

  ! Context 2
  call gpc_context(2)
  call gpc_physor(1.5, 0.8)
  call gpc_orel(-0.70, 0.0)
  call gpc_area2d(6.0, 5.0)
  call gpc_color(COLOR_RED)
  call gpc_graf("%-4.1f", 0.0, 1.0, 10.0, "%-4.1f", 0.0, 0.5, 3.0)

  call gpc_simuplot(0)

  call gpc_end()
  call gpc_stop()
  print *, "Generated multi_y.svg successfully"
end program multi_y
