#set(XC16_SEARCH_PATH "/opt/microchip/xc16")
set(CMAKE_TOOLCHAIN_FILE CMake/FindXC16.cmake)

cmake_minimum_required(VERSION 3.14)
project(PICo24_Demo C ASM)

#set(XC16_DISABLE_DEFAULT_LINKER_SCRIPT 1)
#include(CMake/FindXC16.cmake)

add_compile_options(-std=gnu99 -O2 -mlarge-code -mlarge-data -mlarge-scalar)
#add_link_options(-Wl,--gc-sections,--no-select-objects,--heap=0,--stack=128)


#
set(CPM_DOWNLOAD_VERSION 0.32.0)

if(CPM_SOURCE_CACHE)
    set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else()
    set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif()

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD
            https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
            ${CPM_DOWNLOAD_LOCATION}
            )
endif()

include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage(
        NAME PICo24
        GITHUB_REPOSITORY SudoMaker/PICo24
        VERSION 0.0.2
        GIT_SHALLOW ON
        GIT_TAG 4ac0ebdc6d9ce02ed2c6330a89d3c79c31bbb70f
)

message("-- PICo24 Source ssDir: ${PICo24_SRC_DIR}")

function(add_demo NAME FILES)
    add_executable(PICo24_Demo_${NAME} ${FILES})
    pico24_target_configure(PICo24_Demo_${NAME} "PotatoPi_PICo24" "UART;SPI;TIMER;USB_DEVICE;USB_HOST;USB_DEVICE_CDC_ACM;USB_DEVICE_MSD" "FreeRTOS")

    set(HEX_FILE ${PROJECT_BINARY_DIR}/PICo24_Demo_${NAME}.hex)

    add_custom_command(TARGET PICo24_Demo_${NAME} POST_BUILD
            COMMAND ${XC16_BIN2HEX} PICo24_Demo_${NAME} -a
            COMMENT "Building iHEX: ${HEX_FILE}")
endfunction()

add_demo(RetroWave_USB RetroWave_USB.c)
add_demo(LwIP_PPP LwIP_PPP.c)
add_demo(MSD_16x MSD_16x.c)
add_demo(Lua_Hello Lua_Hello.c)
