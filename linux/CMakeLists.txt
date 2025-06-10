cmake_minimum_required(VERSION 3.22)

project(LinuxUvcTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

option(BUILD_SDL_MODULE "Build the SDL helper module" OFF)
option(BUILD_SOURCES_MODULE "Build the Sources module" OFF)
option(BUILD_JPEGBENCHMARK_APP "Build Jpegbenchmark app 1" OFF)
option(BUILD_SDL_PREVIEW_APP "Build SDL UVC preview sample app" OFF)
option(BUILD_GTK_PREVIEW_APP "Build GTK UVC preview sample app" ON)

include_directories(
        ../libausbc/src/main/jni/UVCCamera/
)

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

if(BUILD_SDL_PREVIEW_APP AND BUILD_SOURCES_MODULE AND BUILD_SDL_MODULE)
    add_subdirectory(apps/sdl_preview)
elseif(BUILD_SDL_PREVIEW_APP)
    message(WARNING "Skipping UvcCapture build because SDL helper or Sources module is not enabled.")
endif()

if(BUILD_GTK_PREVIEW_APP)
    add_subdirectory(apps/gtk_preview)
# elseif(BUILD_GTK_PREVIEW_APP)
#     message(WARNING "Skipping GtkPreview build because Sources module is not enabled.")
endif()