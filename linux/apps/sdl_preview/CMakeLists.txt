cmake_minimum_required(VERSION 3.22)

if(BUILD_SDL_PREVIEW_APP)
        find_package(SDL2 REQUIRED)
        find_package(Threads REQUIRED)

        add_executable(sdk_preview
                uvccapture.cpp
                ../../../libausbc/src/main/jni/UVCCamera/ImageUseCases.cpp
                )

        target_link_libraries(
                sdk_preview
                Sources
                SdlHelper
                ${SDL2_LIBRARIES}
                ${CMAKE_THREAD_LIBS_INIT}
                )
endif()                