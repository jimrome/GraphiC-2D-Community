!===============================================================================
! GraphiC Fortran 2003 Suite: logtest
! Logarithmic and Semi-Log Multi-Decade Coordinate Axes
!
! How to Compile & Run (from GraphiC repository root):
!   make fortran_examples
!   ./fortran/bin/logtest
!
! Direct Command-Line Compilation:
!   gfortran -O2 -I fortran/bin -J fortran/bin logtest.f90 \
!       fortran/bin/graphic_iso.o fortran/bin/graphic.o lib/libgraphic.a -lm -o fortran/bin/logtest
!   ./fortran/bin/logtest
!
! Output:
!   logtest.svg (Vector SVG plot)
!===============================================================================

program logtest
  use graphic
  implicit none

  integer, parameter :: NPTS = 50
  real :: freq(NPTS), gain(NPTS)
  real :: f, w, h
  integer :: i

  do i = 1, NPTS
    f = 10.0 ** (1.0 + real(i - 1) * 4.0 / real(NPTS - 1))
    w = f / 1000.0
    h = 1.0 / sqrt(1.0 + w * w)
    freq(i) = f
    gain(i) = 20.0 * log10(h)
  end do

  call gpc_begin(DRIVER_SVG, "logtest.svg")
  call gpc_page(8.0, 6.0)
  call gpc_start(COLOR_WHITE)
  call gpc_area2d(5.5, 4.0)
  call gpc_physor(1.5, 1.0)
  call gpc_box()
  call gpc_title("Fortran 2003 Frequency Response", "Frequency (Hz)", "Gain (dB)")
  call gpc_xlog(10.0, 100000.0, "%-1.1f", -40.0, 10.0, 0.0)

  call gpc_color(COLOR_RED)
  call gpc_curve(freq, gain, NPTS, 0)

  call gpc_end()
  call gpc_stop()
  print *, "Generated logtest.svg successfully"
end program logtest
