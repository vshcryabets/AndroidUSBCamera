cmake_minimum_required(VERSION 3.22)

if(BUILD_LIBUSB)
    set(LIBUSB ${CMAKE_CURRENT_LIST_DIR}/../../)

    add_compile_definitions(ACCESS_RAW_DESCRIPTORS)

    add_library(LibUsb STATIC
        ${LIBUSB}/libusb/core.c
        ${LIBUSB}/libusb/descriptor.c
        ${LIBUSB}/libusb/hotplug.c
        ${LIBUSB}/libusb/io.c
        ${LIBUSB}/libusb/sync.c
        ${LIBUSB}/libusb/strerror.c
        ${LIBUSB}/libusb/os/android_usbfs.c
        ${LIBUSB}/libusb/os/poll_posix.c
        ${LIBUSB}/libusb/os/threads_posix.c
        ${LIBUSB}/libusb/os/android_netlink.c
        )

    target_link_libraries(LibUsb
            common)

    target_include_directories(LibUsb PUBLIC
            $<BUILD_INTERFACE:${LIBUSB}/libusb/>
            $<BUILD_INTERFACE:${LIBUSB}/>
    )
endif()