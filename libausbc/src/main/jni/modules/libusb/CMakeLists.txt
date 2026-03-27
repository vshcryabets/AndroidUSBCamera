cmake_minimum_required(VERSION 3.31)

## Old one libusb 1.0.18
set(LIBUSB ${CMAKE_CURRENT_LIST_DIR}/../../libusb/)
## Latest one 1.0.30
#set(LIBUSB ${CMAKE_CURRENT_LIST_DIR}/../../../../../../submodules/libusb.git/)

add_compile_definitions(ACCESS_RAW_DESCRIPTORS)

add_library(LibUsb STATIC
    ${LIBUSB}/libusb/core.c
    ${LIBUSB}/libusb/descriptor.c
    ${LIBUSB}/libusb/hotplug.c
    ${LIBUSB}/libusb/io.c
    ${LIBUSB}/libusb/sync.c
    ${LIBUSB}/libusb/strerror.c    
    ${LIBUSB}/libusb/os/poll_posix.c # 1.0.18
    #${LIBUSB}/libusb/os/events_posix.c # 1.0.30
    ${LIBUSB}/libusb/os/threads_posix.c
    )

if (TARGET_OS STREQUAL "Android")
    target_sources(LibUsb PRIVATE
    ${LIBUSB}/libusb/os/android_usbfs.c
    ${LIBUSB}/libusb/os/android_netlink.c
    )
    target_compile_definitions(LibUsb PRIVATE OS_ANDROID=1)
elseif (TARGET_OS STREQUAL "Darwin")
    target_sources(LibUsb PRIVATE
    ${LIBUSB}/libusb/os/darwin_usb.c
    )
    target_compile_definitions(LibUsb PRIVATE OS_DARWIN=1)
elseif (TARGET_OS STREQUAL "Linux")
    target_sources(LibUsb PRIVATE
    ${LIBUSB}/libusb/os/linux_netlink.c
    ${LIBUSB}/libusb/os/linux_usbfs.c
    ${LIBUSB}/libusb/os/linux_udev.c
    )    
    target_compile_definitions(LibUsb PRIVATE OS_LINUX=1)
else()
    message(FATAL_ERROR "Unsupported OS: ${TARGET_OS}")
endif()

target_link_libraries(LibUsb
        common)

target_include_directories(LibUsb PUBLIC
        $<BUILD_INTERFACE:${LIBUSB}/libusb/>
        $<BUILD_INTERFACE:${LIBUSB}/
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/>
)

add_executable(ListUsb)
target_sources(ListUsb PRIVATE
    test/list_usb.cpp
)
target_link_libraries(ListUsb
    LibUsb
)