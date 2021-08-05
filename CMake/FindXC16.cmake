if (XC16_TARGET_CPU)
    message("-- FindXC16: CPU set to ${XC16_TARGET_CPU}")
    set(XC16_COMPILER_OPTIONS -mcpu=${XC16_TARGET_CPU})

    if (NOT XC16_DISABLE_DEFAULT_LINKER_SCRIPT)
        message("-- FindXC16: Using default linker script for device ${XC16_TARGET_CPU}")
        set(XC16_LINKER_OPTIONS --script=p${XC16_TARGET_CPU}.gld)
    else()
        message("-- FindXC16: Default linker script disabled")
    endif()
endif()

if (UNIX)
    if (APPLE)
        set(XC16_DEFAULT_SEARCH_PATH /Applications/microchip/xc16)
    else()
        set(XC16_DEFAULT_SEARCH_PATH /opt/microchip/xc16)
    endif()
endif()

if (WIN32)
    set(XC16_DEFAULT_SEARCH_PATH "D:/Microchip/xc16")
endif()

if (NOT XC16_SEARCH_PATH)
    message("-- FindXC16: Variable XC16_SEARCH_PATH isn't defined, default to ${XC16_DEFAULT_SEARCH_PATH}")
    set(XC16_SEARCH_PATH ${XC16_DEFAULT_SEARCH_PATH})
endif()

set(CMAKE_C_LINK_FLAGS "")

file(GLOB_RECURSE XC16_C_COMPILER ${XC16_SEARCH_PATH}/xc16-gcc)
if (XC16_C_COMPILER)
    message("-- FindXC16: Found xc16-gcc at ${XC16_C_COMPILER}")
    set(CMAKE_C_COMPILER    ${XC16_C_COMPILER})
    set(CMAKE_ASM_COMPILER  ${XC16_C_COMPILER})
else()
    message(FATAL_ERROR "xc16-gcc not found")
endif()

file(GLOB_RECURSE XC16_CXX_COMPILER ${XC16_SEARCH_PATH}/xc16-g++)
if (XC16_CXX_COMPILER)
    message("-- FindXC16: Found xc16-g++ at ${XC16_CXX_COMPILER}")
    set(CMAKE_CXX_COMPILER ${XC16_CXX_COMPILER})
else()
    message("-- FindXC16: xc16-g++ not found, C++ support disabled")
endif()

file(GLOB_RECURSE XC16_AR ${XC16_SEARCH_PATH}/xc16-ar)
if (XC16_AR)
    message("-- FindXC16: Found xc16-ar at ${XC16_AR}")
    set(CMAKE_AR ${XC16_AR})
endif()

file(GLOB_RECURSE XC16_NM ${XC16_SEARCH_PATH}/xc16-nm)
if (XC16_NM)
    message("-- FindXC16: Found xc16-nm at ${XC16_NM}")
    set(CMAKE_NM ${XC16_NM})
endif()

file(GLOB_RECURSE XC16_RANLIB ${XC16_SEARCH_PATH}/xc16-ranlib)
if (XC16_RANLIB)
    message("-- FindXC16: Found xc16-ranlib at ${XC16_RANLIB}")
    set(CMAKE_RANLIB ${XC16_RANLIB})
endif()

file(GLOB_RECURSE XC16_OBJCOPY ${XC16_SEARCH_PATH}/xc16-objcopy)
if (XC16_OBJCOPY)
    message("-- FindXC16: Found xc16-objcopy at ${XC16_OBJCOPY}")
    set(CMAKE_OBJCOPY ${XC16_OBJCOPY})
endif()

file(GLOB_RECURSE XC16_OBJDUMP ${XC16_SEARCH_PATH}/xc16-objdump)
if (XC16_OBJDUMP)
    message("-- FindXC16: Found xc16-objdump at ${XC16_OBJDUMP}")
    set(CMAKE_OBJDUMP ${XC16_OBJDUMP})
endif()

file(GLOB_RECURSE XC16_BIN2HEX ${XC16_SEARCH_PATH}/xc16-bin2hex)
if (XC16_BIN2HEX)
    message("-- FindXC16: Found xc16-bin2hex at ${XC16_BIN2HEX}")
endif()

add_compile_options(${XC16_COMPILER_OPTIONS} -fno-lto -fno-strict-aliasing -omf=elf -msmart-io=1 -msfr-warn=off)

add_link_options(-fno-lto -Wl,--no-select-objects,${XC16_LINKER_OPTIONS},--check-sections,--data-init,--pack-data,--handles,--isr,--fill-upper=0,--stackguard=16,--force-link,--smart-io,--report-mem)