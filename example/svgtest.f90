!===============================================================================
! GraphiC Fortran 2003 Suite: svgtest
! Demonstrates:
!   1. Automatic SVG tight cropping with gpc_svgcrop()
!   2. HTTP/HTTPS POST network telemetry push with gpc_svgpush()
!   3. Automatic browser/viewer preview launch with gpc_svgopen()
!
! How to Compile & Run (from GraphiC repository root):
!   make fortran_examples
!   ./fortran/bin/svgtest [--open] [push_url]
!
! Direct Command-Line Compilation:
!   gfortran -O2 -I fortran/bin -J fortran/bin svgtest.f90 \
!       fortran/bin/graphic_iso.o fortran/bin/graphic.o lib/libgraphic.a -lm -o fortran/bin/svgtest
!   ./fortran/bin/svgtest
!
! Output:
!   svgtest.svg (Vector SVG plot)
!   svgtest.html (Interactive Pan/Zoom Viewer)
!===============================================================================

program svgtest
  use graphic
  implicit none

  integer, parameter :: NPTS = 200
  real :: x(NPTS), y(NPTS)
  real :: t
  integer :: i, argc, iarg
  character(len=256) :: arg, push_url
  character(len=256) :: env_url
  logical :: do_open, has_url
  integer :: env_stat

  print *, "=== GraphiC Fortran 2003 SVG Push & Auto-Preview Test (svgtest) ==="

  do_open = .false.
  has_url = .false.
  push_url = ""

  ! Parse command-line arguments
  argc = command_argument_count()
  do iarg = 1, argc
    call get_command_argument(iarg, arg)
    if (trim(arg) == "--open" .or. trim(arg) == "-o") then
      do_open = .true.
    else if (.not. has_url .and. arg(1:1) /= "-") then
      push_url = trim(arg)
      has_url = .true.
    end if
  end do

  if (.not. has_url) then
    call get_environment_variable("GPC_SVG_PUSH_URL", env_url, status=env_stat)
    if (env_stat == 0 .and. len_trim(env_url) > 0) then
      push_url = trim(env_url)
      has_url = .true.
    end if
  end if

  ! Compute damped sinusoidal wave
  do i = 1, NPTS
    t = real(i - 1) * 0.1
    x(i) = t
    y(i) = exp(-0.15 * t) * sin(2.5 * t)
  end do

  ! Initialize plot session to svgtest.svg
  call gpc_begin(DRIVER_SVG, "svgtest.svg")
  call gpc_start(COLOR_WHITE)

  ! 1. Auto-crop with 0.25 inch margin
  call gpc_svgcrop(0.25)

  call gpc_page(8.5, 6.0)
  call gpc_area2d(7.0, 4.5)
  call gpc_physor(1.5, 1.2)
  call gpc_box()

  call gpc_title("Fortran 2003 SVG Auto-Crop, Push & Preview Test", "Time (seconds)", "Signal Amplitude (V)")
  call gpc_graf("%g", 0.0, 2.0, 20.0, "%g", -1.0, 0.5, 1.0)
  call gpc_grid(1)

  call gpc_color(COLOR_BLUE)
  call gpc_curve(x, y, NPTS, 0)

  ! Finalize and flush the SVG plot
  call gpc_end()
  print *, "✔ Plot successfully rendered to 'svgtest.svg' (tightly cropped with gpc_svgcrop)."

  ! 2. Test gpc_svgpush()
  if (has_url) then
    print '(A,A,A)', "Pushing plot to endpoint '", trim(push_url), "' via gpc_svgpush()..."
    call gpc_svgpush(trim(push_url), "svgtest.svg")
    print *, "✔ gpc_svgpush() dispatched successfully."
  else
    print *, "Info: gpc_svgpush() test skipped (pass a URL argument or set GPC_SVG_PUSH_URL)."
    print *, "      Example: ./fortran/bin/svgtest http://localhost:8080/live"
  end if

  ! 3. Test gpc_svgopen()
  if (do_open) then
    print *, "Opening 'svgtest.svg' in default viewer with gpc_svgopen()..."
    call gpc_svgopen("svgtest.svg")
    print *, "✔ gpc_svgopen() dispatched successfully."
  else
    print *, "Info: Run with '--open' to launch your default browser/viewer automatically:"
    print *, "      Example: ./fortran/bin/svgtest --open"
  end if

  call gpc_stop()
  print *, "=== Test Completed Successfully ==="
end program svgtest
