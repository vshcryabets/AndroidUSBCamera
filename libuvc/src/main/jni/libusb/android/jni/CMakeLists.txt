cmake_minimum_required(VERSION 3.22)

set(LIBUSB ${CMAKE_CURRENT_LIST_DIR}/../../)
include_directories(
    ${LIBUSB}/
    ${LIBUSB}/libusb/
)

add_compile_definitions(ACCESS_RAW_DESCRIPTORS)

list(APPEND SOURCES
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
