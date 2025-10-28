cmake_minimum_required(VERSION 3.31)

project(libausbc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILATION_FLAGS} -std=c++17")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${COMPILATION_RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${COMPILATION_DEBUG_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILATION_FLAGS} -std=c99")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} ${COMPILATION_RELEASE_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} ${COMPILATION_DEBUG_FLAGS}")

option(BUILD_IMAGES_MODULE "Build the Images module" ON)
option(BUILD_JNIWRAPPER_MODULE "Build the JniWrapper module" ON)
option(BUILD_LIBUVC "Build the LibUVC library" ON)
option(BUILD_LIBUSB "Build the LibUSB library" ON)
option(BUILD_SOURCES_MODULE "Build Sources module" ON)
option(BUILD_JNI_WRAPPERS "Build JNI wrappers" ON)

set(ANDROID_ARM_NEON TRUE)
set(REQUIRE_SIMD ON)
set(ENABLE_SHARED FALSE)
set(ENABLE_STATIC TRUE)
add_compile_definitions(LIBUVC_HAS_JPEG)

add_subdirectory(./src/main/jni/libjpeg-turbo-3.1.1-static/)
add_subdirectory(./src/main/jni/modules/images)
add_subdirectory(./src/main/jni/modules/sources)
add_subdirectory(./src/main/jni/common/)
add_subdirectory(./src/main/jni/libusb/android/jni/)
add_subdirectory(./src/main/jni/libuvc/android/jni/)
add_subdirectory(./src/main/jni/UVCCamera)
add_subdirectory(./src/main/jni/modules/font)