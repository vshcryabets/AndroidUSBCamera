cmake_minimum_required(VERSION 3.22)

if(BUILD_LIBUVC)
    set(LIBUVC ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../libuvc-git/)

    configure_file(${LIBUVC}/include/libuvc/libuvc_config.h.in
            ${LIBUVC}/include/libuvc/libuvc_config.h
            @ONLY
    )

    add_compile_definitions(LIBUVC_HAS_JPEG)

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
            libJpegStatic)

    target_include_directories(LibUvc PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${LIBUVC}/include>
    )

endif()