# Makefile for Fortran 2D Examples
FC ?= gfortran
FFLAGS ?= -O2 -I../src
LDFLAGS ?= -L../../lib -lgraphic -lm

ifeq ($(OS),Windows_NT)
    ifeq ($(MSYSTEM),)
        RM = del /q 2>nul || exit 0
    else
        RM = rm -f
    endif
else
    RM = rm -f
endif

SRCS = ../src/graphic_iso.f90 ../src/graphic.f90
EXAMPLES = sample d2test logtest bartest erftest multi_y svgtest turbulence

all: $(EXAMPLES)

%: %.f90 $(SRCS)
	$(FC) $(FFLAGS) $(SRCS) $< $(LDFLAGS) -o $@

clean:
	$(RM) $(EXAMPLES) *.mod *.svg *.html *.tkf
