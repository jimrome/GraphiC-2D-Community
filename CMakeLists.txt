cmake_minimum_required(VERSION 3.15)
project(GraphiC_Community VERSION 7.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(GRAPHIC_BUILD_STATIC "Also build static library" ON)
option(BUILD_EXAMPLES "Build C and Fortran examples" ON)

set(GRAPHIC_COMMUNITY_SOURCES
    Source/Arrow.c
    Source/Axesdraw.c
    Source/Barplt.c
    Source/Bothplt.c
    Source/Bplt.c
    Source/Circles.c
    Source/Color.c
    Source/Colormap.c
    Source/Curves.c
    Source/Getxy.c
    Source/Gridopts.c
    Source/Insert.c
    Source/Labset.c
    Source/Legend.c
    Source/Lineattr.c
    Source/Lines.c
    Source/Linplt.c
    Source/Logplt.c
    Source/Lplt.c
    Source/Messagew.c
    Source/Panfil.c
    Source/Pclp.c
    Source/Position.c
    Source/Psfont.c
    Source/Roundoff.c
    Source/Simplt.c
    Source/Statfuns.c
    Source/Svc.c
    Source/Symbols.c
    Source/Tekint.c
    Source/Tplt.c
    Source/Truetype.c
    Source/Ttdisk.c
    Source/Ttfntld.c
    Source/Ttfntplt.c
    Source/baux_portable.c
    Source/community_stubs.c
    Source/gpc_driver_svg.c
    Source/gpcfor_compat.c
    Source/memory_portable.c
)

add_library(graphic SHARED ${GRAPHIC_COMMUNITY_SOURCES})
set_target_properties(graphic PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
target_include_directories(graphic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_compile_definitions(graphic PRIVATE GPC_PORTABLE=1 GPC_COMMUNITY=1)

if(MSVC)
    target_compile_definitions(graphic PRIVATE _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_DEPRECATE)
    target_compile_options(graphic PRIVATE /W3 /wd4996 /wd4244 /wd4305 /wd4018)
else()
    target_compile_options(graphic PRIVATE -Wall -w)
    target_link_libraries(graphic PRIVATE m)
endif()

if(GRAPHIC_BUILD_STATIC)
    add_library(graphic_static STATIC ${GRAPHIC_COMMUNITY_SOURCES})
    target_include_directories(graphic_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
    target_compile_definitions(graphic_static PRIVATE GPC_PORTABLE=1 GPC_COMMUNITY=1)
    if(MSVC)
        target_compile_definitions(graphic_static PRIVATE _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_DEPRECATE)
        target_compile_options(graphic_static PRIVATE /W3 /wd4996 /wd4244 /wd4305 /wd4018)
    else()
        target_compile_options(graphic_static PRIVATE -Wall -w)
        target_link_libraries(graphic_static PRIVATE m)
    endif()
endif()

# Build Examples
if(BUILD_EXAMPLES)
    set(C_EXAMPLES
        Sample c/examples/Sample.c
        D2test c/examples/D2test.c
        Logtest c/examples/Logtest.c
        Bartest c/examples/Bartest.c
        Erftest c/examples/Erftest.c
        Multi_y c/examples/Multi_y.c
        Svgtest c/examples/Svgtest.c
        Turbulence c/examples/Turbulence.c
    )
    while(C_EXAMPLES)
        list(POP_FRONT C_EXAMPLES ex_name ex_src)
        add_executable(${ex_name} ${ex_src})
        target_link_libraries(${ex_name} PRIVATE graphic)
        target_include_directories(${ex_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
        if(NOT MSVC)
            target_link_libraries(${ex_name} PRIVATE m)
        endif()
    endwhile()

    include(CheckLanguage)
    check_language(Fortran)
    if(CMAKE_Fortran_COMPILER)
        enable_language(Fortran)
        add_library(graphic_fortran STATIC
            Source/graphic_iso.f90
            Source/graphic.f90
        )
        set_target_properties(graphic_fortran PROPERTIES
            Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/modules
        )
        target_include_directories(graphic_fortran PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/modules)
        target_link_libraries(graphic_fortran PRIVATE graphic)

        set(F_EXAMPLES
            f_sample   fortran/examples/sample.f90
            f_d2test   fortran/examples/d2test.f90
            f_logtest  fortran/examples/logtest.f90
            f_bartest  fortran/examples/bartest.f90
            f_erftest  fortran/examples/erftest.f90
            f_multi_y  fortran/examples/multi_y.f90
            f_svgtest  fortran/examples/svgtest.f90
            f_turbulence fortran/examples/turbulence.f90
        )
        while(F_EXAMPLES)
            list(POP_FRONT F_EXAMPLES f_ex_name f_ex_src)
            add_executable(${f_ex_name} ${f_ex_src})
            target_link_libraries(${f_ex_name} PRIVATE graphic_fortran graphic)
            target_include_directories(${f_ex_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/modules)
            if(NOT MSVC)
                target_link_libraries(${f_ex_name} PRIVATE m)
            endif()
        endwhile()
    endif()
endif()
