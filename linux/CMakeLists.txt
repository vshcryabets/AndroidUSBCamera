cmake_minimum_required(VERSION 3.22)

project(LinuxUvcTests)

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH ROOT_DIR)
set(UVCCAMERA ${ROOT_DIR}/libausbc/src/main/jni/UVCCamera)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.3
)
FetchContent_MakeAvailable(Catch2)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package (JPEG REQUIRED)

#set(USE_TURBOJPEG 1)
set(USE_LIBJPEG 1)

include_directories(
        ${UVCCAMERA}/include/
        ./include/
)

find_package(SDL2 REQUIRED)

add_executable(uvccapture
        ./src/SdlTools.cpp
        ./src/UvcCamera.cpp
        ./src/uvccapture.cpp
        ${UVCCAMERA}/src/ImageUseCases.cpp
        )

target_link_libraries(
        uvccapture
        ${SDL2_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})


add_executable(jpegbenchmark
        ./src/jpegdecodebenchmark.cpp
        ${UVCCAMERA}/src/ImageUseCases.cpp
        ${UVCCAMERA}/src/JpegBenchmark.cpp
        ${UVCCAMERA}/src/ProgressObserver.cpp
        )

if(USE_TURBOJPEG)
        target_compile_definitions(jpegbenchmark PRIVATE -DUSE_TURBOJPEG)
        add_library(turbojpeg SHARED IMPORTED)
        set_target_properties(turbojpeg PROPERTIES
                IMPORTED_LOCATION /usr/lib/libturbojpeg.so)
        target_link_libraries(
                jpegbenchmark
                turbojpeg
                ${CMAKE_THREAD_LIBS_INIT})         
endif()

if (USE_LIBJPEG)
        target_compile_definitions(jpegbenchmark PRIVATE -DUSE_LIBJPEG)
        target_link_libraries(
                jpegbenchmark
                ${JPEG_LIBRARIES}
                ${CMAKE_THREAD_LIBS_INIT})        
endif()

##
## USB Audio devices tests
##

pkg_check_modules(LibUSB REQUIRED libusb-1.0)

if(LibUSB_FOUND)
  include_directories(${LibUSB_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "LibUSB not found. Please install libusb-dev (or equivalent) on your system.")
endif()

add_executable(uaccapture
        ./src/uaccapture.cpp
        ./src/UacSource.cpp
        )

target_link_libraries(uaccapture ${LibUSB_LIBRARIES})
        
# Enable testing with CTest
enable_testing()

# add_test(NAME DemoTests COMMAND catch2_tests)
add_executable(tests test/test_catch2.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)