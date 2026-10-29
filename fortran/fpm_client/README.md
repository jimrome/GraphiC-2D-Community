# Minimal `fpm` Client Consuming GraphiC

This directory contains a standalone Fortran Package Manager (`fpm`) client project that consumes **GraphiC 2D Community Edition** as an external git dependency.

## Project Structure

```text
fpm_client/
├── fpm.toml
└── app/
    └── main.f90
```

## `fpm.toml`

```toml
name = "graphic_client_demo"
version = "0.1.0"
license = "MIT"
author = "James Rome, Sc.D."
description = "Minimal standalone Fortran client project consuming GraphiC as an fpm dependency"

[dependencies]
graphic = { git = "https://github.com/jimrome/GraphiC-2D-Community.git" }
```

## Build & Run

From this directory (`fortran/fpm_client`), run:

```bash
fpm run
```

`fpm` will automatically fetch `GraphiC-2D-Community`, compile the C core and Fortran `iso_c_binding` modules (`graphic_iso` and `graphic`), link the client executable, and render `damped_oscillator.svg` (along with the interactive `damped_oscillator.html` viewer).
