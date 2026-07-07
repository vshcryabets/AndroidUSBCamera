cmake_minimum_required(VERSION 3.31)

set(LIBUVC ${CMAKE_CURRENT_LIST_DIR}/../../../../../../submodules/libuvc.git/)

set(LIBUVC_CUSTOM_DEFINITIONS [=[
/* Project-specific overrides */
#define MY_DEFINE 1
#define MY_OTHER_DEFINE 42
]=])

configure_file(
    ${LIBUVC}/include/libuvc/libuvc_config.h.in
    ${CMAKE_CURRENT_LIST_DIR}/include/libuvc/libuvc_config.h
    @ONLY
)

file(APPEND
    ${CMAKE_CURRENT_LIST_DIR}/include/libuvc/libuvc_config.h
    "\n${LIBUVC_CUSTOM_DEFINITIONS}\n"
)

add_library(LibUvc STATIC
        ${LIBUVC}/src/ctrl.c
        ${LIBUVC}/src/device.c
        ${LIBUVC}/src/diag.c
        ${LIBUVC}/src/frame.c
        ${LIBUVC}/src/frame-mjpeg.c
        ${LIBUVC}/src/init.c
        ${LIBUVC}/src/stream.c
)

target_link_libraries(LibUvc PUBLIC
        LibUsb
        )
if (TARGET_OS STREQUAL "Android")
target_link_libraries(LibUvc PUBLIC
        libJpegStatic)
else()
    find_package(JPEG REQUIRED)

    if (TARGET JPEG::JPEG)
        target_link_libraries(LibUvc PUBLIC
                JPEG::JPEG)
    else()
        target_include_directories(LibUvc PUBLIC ${JPEG_INCLUDE_DIRS})
        target_link_libraries(LibUvc PUBLIC
                ${JPEG_LIBRARIES})
    endif()
endif()

target_compile_definitions(LibUvc PUBLIC 
    LIBUVC_HAS_JPEG
    UVC_DEBUGGING # Enable debug logs
    )

target_include_directories(LibUvc PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${LIBUVC}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

add_executable(checkCameraCapabilities
    test/checkCameraCapabilities.cpp
    )
target_link_libraries(checkCameraCapabilities PRIVATE
    LibUvc
    )

add_executable(readFrames
    test/readFrames.cpp
    )
target_link_libraries(readFrames PRIVATE
    LibUvc
    common
    )