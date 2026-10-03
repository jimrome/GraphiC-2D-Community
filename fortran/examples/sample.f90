!===============================================================================
! GraphiC Fortran 2003 Suite: sample
! Composite Multi-Panel Layout with Subplots and Inset Frames
!
! How to Compile & Run (from GraphiC repository root):
!   make fortran_examples
!   ./fortran/bin/sample
!
! Direct Command-Line Compilation:
!   gfortran -O2 -I fortran/bin -J fortran/bin sample.f90 \
!       fortran/bin/graphic_iso.o fortran/bin/graphic.o lib/libgraphic.a -lm -o fortran/bin/sample
!   ./fortran/bin/sample
!
! Output:
!   sample.svg (Vector SVG plot)
!===============================================================================

program sample
  use graphic
  implicit none

  integer, parameter :: N = 100
  real :: x(N), y1(N), y2(N)
  real :: v
  integer :: i

  do i = 1, N
    v = real(i - 1) * 0.1
    x(i) = v
    y1(i) = sin(v)
    y2(i) = cos(v)
  end do

  call gpc_begin(DRIVER_SVG, "sample.svg")
  call gpc_page(8.5, 6.5)
  call gpc_start(COLOR_WHITE)
  call gpc_area2d(6.0, 4.5)
  call gpc_physor(1.5, 1.0)
  call gpc_box()
  call gpc_title("Multi-Waveform Analysis (Fortran Sample)", "Phase (rad)", "Signal Amplitude")
  call gpc_graf("%3.1f", 0.0, 1.0, 10.0, "%3.1f", -1.0, 0.5, 1.0)

  call gpc_color(COLOR_BLUE)
  call gpc_curve(x, y1, N, 0)

  call gpc_color(COLOR_RED)
  call gpc_curve(x, y2, N, 0)

  call gpc_end()
  call gpc_stop()
  print *, "Generated sample.svg successfully"
end program sample
