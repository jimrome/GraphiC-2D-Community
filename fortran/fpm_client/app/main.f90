!===============================================================================
! Minimal Client Project Consuming GraphiC as an fpm Dependency
!
! Build & Run:
!   fpm run
!
! Output:
!   damped_oscillator.svg
!===============================================================================
program graphic_client_demo
  use graphic
  implicit none

  integer, parameter :: N = 200
  real :: t(N), y(N), env_pos(N), env_neg(N)
  real :: dt, gamma, omega
  integer :: i

  dt = 10.0 / real(N - 1)
  gamma = 0.25
  omega = 3.0

  do i = 1, N
    t(i) = real(i - 1) * dt
    env_pos(i) = exp(-gamma * t(i))
    env_neg(i) = -env_pos(i)
    y(i) = env_pos(i) * cos(omega * t(i))
  end do

  call gpc_begin(DRIVER_SVG, "damped_oscillator.svg")
  call gpc_page(8.5, 6.0)
  call gpc_start(COLOR_WHITE)
  call gpc_svgcrop(0.25)

  call gpc_area2d(6.0, 4.2)
  call gpc_physor(1.4, 1.1)
  call gpc_box()
  call gpc_title("Damped Harmonic Oscillator (fpm Client Demo)", &
                 "Time t (s)", "Displacement y(t)")
  call gpc_graf("%3.1f", 0.0, 2.0, 10.0, "%4.1f", -1.0, 0.5, 1.0)
  call gpc_grid(1)

  ! Envelope curves (dashed red)
  call gpc_color(COLOR_RED)
  call gpc_dashf(STYLE_DASHED)
  call gpc_curve(t, env_pos, N, 0)
  call gpc_curve(t, env_neg, N, 0)

  ! Underdamped oscillation (solid blue)
  call gpc_color(COLOR_BLUE)
  call gpc_dashf(STYLE_SOLID)
  call gpc_curve(t, y, N, 0)

  call gpc_end()
  call gpc_stop()
  print *, "Generated damped_oscillator.svg successfully"
end program graphic_client_demo
