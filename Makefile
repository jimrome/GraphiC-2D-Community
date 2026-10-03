# Makefile for GraphiC 2D Community Edition
CC ?= cc
AR ?= ar
ARFLAGS ?= rcs

CFLAGS ?= -O2 -IInclude -DGPC_PORTABLE=1 -DGPC_COMMUNITY=1 -w
LDFLAGS ?= -lm

ifeq ($(origin FC),default)
    FC = gfortran
endif
FC ?= gfortran
FFLAGS ?= -O2 -w

LIB_DIR = lib
BIN_DIR = bin
OBJ_DIR = build

ifeq ($(OS),Windows_NT)
    SHLIB_EXT = .dll
    SHLIB_FLAG = -shared
    ifeq ($(MSYSTEM),)
        SHELL := cmd.exe
        .SHELLFLAGS := /c
        MKDIR_OBJ = @if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
        MKDIR_LIB = @if not exist $(LIB_DIR) mkdir $(LIB_DIR)
        MKDIR_BIN = @if not exist $(BIN_DIR) mkdir $(BIN_DIR)
        RM_ALL = @if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR) & if exist $(LIB_DIR) rmdir /s /q $(LIB_DIR) & if exist $(BIN_DIR) rmdir /s /q $(BIN_DIR) & del /q *.svg *.html *.tkf 2>nul || exit 0
    else
        MKDIR_OBJ = @mkdir -p $(OBJ_DIR)
        MKDIR_LIB = @mkdir -p $(LIB_DIR)
        MKDIR_BIN = @mkdir -p $(BIN_DIR)
        RM_ALL = @rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) *.svg *.html *.tkf
    endif
else
    UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)
    ifeq ($(UNAME_S),Darwin)
        SHLIB_EXT = .dylib
        SHLIB_FLAG = -dynamiclib
    else
        SHLIB_EXT = .so
        SHLIB_FLAG = -shared
    endif
    MKDIR_OBJ = @mkdir -p $(OBJ_DIR)
    MKDIR_LIB = @mkdir -p $(LIB_DIR)
    MKDIR_BIN = @mkdir -p $(BIN_DIR)
    RM_ALL = @rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) *.svg *.html *.tkf
endif

STATIC_LIB = $(LIB_DIR)/libgraphic.a
DYNAMIC_LIB = $(LIB_DIR)/libgraphic$(SHLIB_EXT)

SRCS = \
	Source/Arrow.c \
	Source/Axesdraw.c \
	Source/Barplt.c \
	Source/Bothplt.c \
	Source/Bplt.c \
	Source/Circles.c \
	Source/Color.c \
	Source/Colormap.c \
	Source/Curves.c \
	Source/Getxy.c \
	Source/Gridopts.c \
	Source/Insert.c \
	Source/Labset.c \
	Source/Legend.c \
	Source/Lineattr.c \
	Source/Lines.c \
	Source/Linplt.c \
	Source/Logplt.c \
	Source/Lplt.c \
	Source/Messagew.c \
	Source/Panfil.c \
	Source/Pclp.c \
	Source/Position.c \
	Source/Psfont.c \
	Source/Roundoff.c \
	Source/Simplt.c \
	Source/Statfuns.c \
	Source/Svc.c \
	Source/Symbols.c \
	Source/Tekint.c \
	Source/Tplt.c \
	Source/Truetype.c \
	Source/Ttdisk.c \
	Source/Ttfntld.c \
	Source/Ttfntplt.c \
	Source/baux_portable.c \
	Source/community_stubs.c \
	Source/gpc_driver_svg.c \
	Source/gpcfor_compat.c \
	Source/memory_portable.c

OBJS = $(patsubst Source/%.c,$(OBJ_DIR)/%.o,$(SRCS))

C_EXAMPLES = \
	$(BIN_DIR)/Sample \
	$(BIN_DIR)/D2test \
	$(BIN_DIR)/Logtest \
	$(BIN_DIR)/Bartest \
	$(BIN_DIR)/Erftest \
	$(BIN_DIR)/Multi_y \
	$(BIN_DIR)/Svgtest \
	$(BIN_DIR)/Turbulence

FORTRAN_OBJS = $(OBJ_DIR)/graphic_iso.o $(OBJ_DIR)/graphic.o
FORTRAN_EXAMPLES = \
	$(BIN_DIR)/f_sample \
	$(BIN_DIR)/f_d2test \
	$(BIN_DIR)/f_logtest \
	$(BIN_DIR)/f_bartest \
	$(BIN_DIR)/f_erftest \
	$(BIN_DIR)/f_multi_y \
	$(BIN_DIR)/f_svgtest \
	$(BIN_DIR)/f_turbulence

# Detect if Fortran compiler is available
HAS_FC := $(shell $(FC) --version 2>nul || $(FC) --version 2>/dev/null)
ifneq ($(HAS_FC),)
all: $(STATIC_LIB) $(DYNAMIC_LIB) $(C_EXAMPLES) fortran
else
all: $(STATIC_LIB) $(DYNAMIC_LIB) $(C_EXAMPLES)
endif

$(OBJ_DIR)/%.o: Source/%.c
	$(MKDIR_OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(STATIC_LIB): $(OBJS)
	$(MKDIR_LIB)
	$(AR) $(ARFLAGS) $@ $^

$(DYNAMIC_LIB): $(OBJS)
	$(MKDIR_LIB)
	$(CC) $(SHLIB_FLAG) $(OBJS) $(LDFLAGS) -o $@

# C Example rules
$(BIN_DIR)/%: c/examples/%.c $(STATIC_LIB)
	$(MKDIR_BIN)
	$(CC) $(CFLAGS) $< $(STATIC_LIB) $(LDFLAGS) -o $@

# Fortran compilation rules
$(OBJ_DIR)/graphic_iso.o: Source/graphic_iso.f90
	$(MKDIR_OBJ)
	$(FC) $(FFLAGS) -c $< -J $(OBJ_DIR) -o $@

$(OBJ_DIR)/graphic.o: Source/graphic.f90 $(OBJ_DIR)/graphic_iso.o
	$(MKDIR_OBJ)
	$(FC) $(FFLAGS) -c $< -I $(OBJ_DIR) -J $(OBJ_DIR) -o $@

$(BIN_DIR)/f_%: fortran/examples/%.f90 $(FORTRAN_OBJS) $(STATIC_LIB)
	$(MKDIR_BIN)
	$(FC) $(FFLAGS) -I $(OBJ_DIR) $< $(FORTRAN_OBJS) $(STATIC_LIB) $(LDFLAGS) -o $@

fortran: $(STATIC_LIB) $(FORTRAN_OBJS) $(FORTRAN_EXAMPLES)

clean:
	$(RM_ALL)
