cmake_minimum_required(VERSION 3.22)

project(LinuxUvcTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

option(BUILD_IMAGES_MODULE "Build the Images module" ON)
option(BUILD_SDL_MODULE "Build the SDL helper module" ON)
option(BUILD_SOURCES_MODULE "Build the Sources module" ON)
option(BUILD_JPEGBENCHMARK_APP "Build Jpegbenchmark app 1" OFF)
option(BUILD_UVCCAPTURE_APP "Build UvcCapture app" ON)

if(BUILD_IMAGES_MODULE)
    add_subdirectory(../libausbc/src/main/jni/modules/images modules/images)
endif()

if(BUILD_SDL_MODULE)
    add_subdirectory(modules/sdl)
endif()

if(BUILD_SOURCES_MODULE)
    add_subdirectory(modules/sources)
endif()

# Check if required modules are built before adding apps
if(BUILD_JPEGBENCHMARK_APP)
    add_subdirectory(apps/jpegbenchmark)
else()
    message(WARNING "Skipping Jpegbenchmark - not enabled.")
endif()

if(BUILD_UVCCAPTURE_APP AND BUILD_SOURCES_MODULE AND BUILD_SDL_MODULE)
    add_subdirectory(apps/uvccapture)
elseif(BUILD_UVCCAPTURE_APP)
    message(WARNING "Skipping UvcCapture build because SDL helper or Sources module is not enabled.")
endif()




        
