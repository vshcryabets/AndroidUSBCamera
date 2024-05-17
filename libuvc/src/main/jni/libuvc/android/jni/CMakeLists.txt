cmake_minimum_required(VERSION 3.22)

set(LIBUVC ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../libuvc-git/)

configure_file(${LIBUVC}/include/libuvc/libuvc_config.h.in
        ${LIBUVC}/include/libuvc/libuvc_config.h
        @ONLY
)

add_compile_definitions(LIBUVC_HAS_JPEG)

include_directories(
    ${LIBUVC}/include/
)

list(APPEND SOURCES
	${LIBUVC}/src/ctrl.c
    ${LIBUVC}/src/device.c
    ${LIBUVC}/src/diag.c
    ${LIBUVC}/src/frame.c
    ${LIBUVC}/src/frame-mjpeg.c
    ${LIBUVC}/src/init.c
    ${LIBUVC}/src/stream.c
    )
