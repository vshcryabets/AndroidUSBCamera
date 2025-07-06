cmake_minimum_required(VERSION 3.28)

project(LinuxUvcTests LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

option(BUILD_U8X8FONT_MODULE "Build U8X8 fonts" ON)
option(BUILD_SDL_MODULE "Build the SDL helper module" OFF)
option(BUILD_SOURCES_MODULE_V4L2 "Build the Sources module with Video for Linux v2" OFF)
option(BUILD_IMAGES_MODULE "Build the Images module" ON)
option(BUILD_SOURCES_MODULE "Build the Sources module" ON)
option(BUILD_ENCODERS_MODULE "Build the Encoders module" ON)
option(BUILD_JPEGBENCHMARK_APP "Build Jpegbenchmark app 1" OFF)
option(BUILD_SDL_PREVIEW_APP "Build SDL UVC preview sample app" OFF)
option(BUILD_GTK_PREVIEW_APP "Build GTK UVC preview sample app" ON)
option(BUILD_CATCH2_TESTS "Build Catch2 tests" ON)
option(BUILD_X264_TEST_APP "Build X264 test app" ON)


include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.3
)
FetchContent_MakeAvailable(Catch2)

if(BUILD_IMAGES_MODULE)
    add_subdirectory(../libausbc/src/main/jni/modules/images modules/images)
endif()

if(BUILD_SDL_MODULE)
    add_subdirectory(modules/sdl)
endif()

if(BUILD_SOURCES_MODULE)
    add_subdirectory(../libausbc/src/main/jni/modules/sources modules/sources)
endif()

if(BUILD_ENCODERS_MODULE)
    add_subdirectory(../libausbc/src/main/jni/modules/encoders modules/encoders)
endif()

if(BUILD_U8X8FONT_MODULE)
    add_subdirectory(modules/font)
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

if(BUILD_GTK_PREVIEW_APP AND BUILD_SOURCES_MODULE AND BUILD_U8X8FONT_MODULE)
    add_subdirectory(apps/gtk_preview)
elseif(BUILD_GTK_PREVIEW_APP)
    message(WARNING "Skipping GtkPreview build because Sources or U8X8 module is not enabled.")
endif()

if(BUILD_X264_TEST_APP AND BUILD_SOURCES_MODULE)
    add_subdirectory(apps/x264test)
else()
    message(WARNING "Skipping X264 test app build because Sources module is not enabled.")
endif()