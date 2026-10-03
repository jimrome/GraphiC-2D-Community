!==============================================================================
! GraphiC Modern Fortran Suite: Continuous Color Scale Plots (turbulence)
!
! Demonstrates:
!   - Continuous Scientific Colormaps (Viridis)
!   - Querying shading color for each individual data point
!   - TrueType Font rendering (Arial.ttf)
!   - Continuous SVG Gradient Color Bar (gpc_colorkey_gradient)
!   - 7,850 data points colored continuously by log10 density
!==============================================================================

program turbulence_demo
  use graphic
  implicit none

  integer, parameter :: NPTS = 7850
  real :: x(NPTS), y(NPTS), z(NPTS)
  real :: zmin, zmax, u, x_val, base_y, sigma_y, burst, y_val
  real :: norm_y, temp_factor, z_mean, z_val
  real :: u1, u2, g_val
  integer :: i
  integer(8) :: seed

  seed = 123456789_8
  zmin = -0.8
  zmax = 0.8

  ! Generate 7,850 continuous synthetic data points with smooth physical decay
  do i = 1, NPTS
    ! Box-Muller gaussian
    seed = iand(seed * 1103515245_8 + 12345_8, 2147483647_8)
    u1 = max(1.0e-7, real(seed) / 2147483647.0)
    seed = iand(seed * 1103515245_8 + 12345_8, 2147483647_8)
    u2 = real(seed) / 2147483647.0
    g_val = sqrt(-2.0 * log(u1)) * cos(6.2831853 * u2)

    seed = iand(seed * 1103515245_8 + 12345_8, 2147483647_8)
    u = real(seed) / 2147483647.0

    seed = iand(seed * 1103515245_8 + 12345_8, 2147483647_8)
    if (real(seed) / 2147483647.0 < 0.45) then
      x_val = 5.0 + 55.0 * (u ** 1.15) + 2.0 * g_val
    else
      x_val = 20.0 + 228.0 * (u ** 1.30) + 2.0 * g_val
    end if
    if (x_val < 5.0) x_val = 5.0
    if (x_val > 249.0) x_val = 249.0

    base_y = 0.22 + 0.95 * exp(-x_val / 36.0) + 0.18 * exp(-x_val / 130.0)
    sigma_y = 0.045 + 0.22 * exp(-x_val / 40.0)

    burst = 0.0
    seed = iand(seed * 1103515245_8 + 12345_8, 2147483647_8)
    if (real(seed) / 2147483647.0 < 0.28 * exp(-x_val / 32.0)) then
      seed = iand(seed * 1103515245_8 + 12345_8, 2147483647_8)
      burst = -0.35 * log(max(1.0e-5, real(seed) / 2147483647.0))
    end if

    y_val = base_y + sigma_y * g_val + burst
    if (y_val < 0.14) then
      y_val = 0.14 + 0.06 * exp((y_val - 0.14) / 0.06)
    end if
    if (y_val > 1.58) then
      y_val = 1.58 - 0.06 * exp((1.58 - y_val) / 0.06)
    end if

    norm_y = (y_val - 0.14) / 1.44
    if (norm_y < 0.0) norm_y = 0.0
    if (norm_y > 1.0) norm_y = 1.0
    temp_factor = exp(-x_val / 65.0)
    z_mean = -0.76 + 1.28 * (norm_y ** 0.75) + 0.26 * temp_factor

    seed = iand(seed * 1103515245_8 + 12345_8, 2147483647_8)
    u1 = max(1.0e-7, real(seed) / 2147483647.0)
    seed = iand(seed * 1103515245_8 + 12345_8, 2147483647_8)
    u2 = real(seed) / 2147483647.0
    z_val = z_mean + 0.06 * (sqrt(-2.0 * log(u1)) * cos(6.2831853 * u2))
    if (z_val < zmin) z_val = zmin
    if (z_val > zmax) z_val = zmax

    x(i) = x_val
    y(i) = y_val
    z(i) = z_val
  end do

  ! Start plot session
  call gpc_begin('S', 'turbulence_f.svg')
  call gpc_start(COLOR_WHITE)

  ! Load TrueType font Arial.ttf
  call gpc_font('Arial.ttf', char(200))
  call gpc_fillfont(1)

  call gpc_page(9.5, 6.5)
  call gpc_physor(1.35, 1.0)
  call gpc_area2d(5.8, 4.5)

  ! Graph axis names
  call gpc_color(COLOR_BLACK)
  call gpc_xname(char(200) // 'Temperature')
  call gpc_yname(char(200) // 'Transport')

  ! Cartesian grid and axes
  call gpc_numht(0.12)
  call gpc_graf('%-1.0f', 0.0, 50.0, 250.0, '%-1.1f', 0.0, 0.2, 1.6)

  ! Plot all 7,850 points colored continuously via Viridis colormap
  call gpc_scatter(x, y, z, zmin, zmax, 12, 0.035, GPC_COLORMAP_VIRIDIS)

  ! Draw continuous SVG Gradient Color Bar on the right
  call gpc_colorkey_gradient(zmin, zmax, GPC_COLORMAP_VIRIDIS, &
                             7.5, 1.0, 7.8, 5.5, &
                             8, '%-3.1f', char(200), 0.11, 0)

  ! Label for the color bar
  call gpc_prtfnt(8.35, 3.25, char(200) // 'log]10@ Density', 0.13, 90)

  call gpc_hardcopy('n')
  call gpc_end()
  call gpc_stop()
end program turbulence_demo
