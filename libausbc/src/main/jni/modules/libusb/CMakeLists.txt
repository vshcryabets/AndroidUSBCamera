cmake_minimum_required(VERSION 3.31)

set(LIBUSB ${CMAKE_CURRENT_LIST_DIR}/../../libusb/)

add_compile_definitions(ACCESS_RAW_DESCRIPTORS)



add_library(LibUsb STATIC
    ${LIBUSB}/libusb/core.c
    ${LIBUSB}/libusb/descriptor.c
    ${LIBUSB}/libusb/hotplug.c
    ${LIBUSB}/libusb/io.c
    ${LIBUSB}/libusb/sync.c
    ${LIBUSB}/libusb/strerror.c    
    ${LIBUSB}/libusb/os/poll_posix.c
    ${LIBUSB}/libusb/os/threads_posix.c
    )

if (TARGET_OS STREQUAL "Android")
    target_sources(LibUsb PRIVATE
    ${LIBUSB}/libusb/os/android_usbfs.c
    ${LIBUSB}/libusb/os/android_netlink.c
    )
elseif (TARGET_OS STREQUAL "Darwin")
    target_sources(LibUsb PRIVATE
    ${LIBUSB}/libusb/os/darwin_usb.c
    )
elseif (TARGET_OS STREQUAL "Linux")
    target_sources(LibUsb PRIVATE
    ${LIBUSB}/libusb/os/linux-netlink.c
    ${LIBUSB}/libusb/os/linux_usbfs.c
    ${LIBUSB}/libusb/os/linux_udev.c
    )    
endif()

target_link_libraries(LibUsb
        common)

target_include_directories(LibUsb PUBLIC
        $<BUILD_INTERFACE:${LIBUSB}/libusb/>
        $<BUILD_INTERFACE:${LIBUSB}/>
)
