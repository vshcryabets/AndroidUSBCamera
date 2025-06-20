cmake_minimum_required(VERSION 3.22)

if(BUILD_GTK_PREVIEW_APP)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(GTK4 REQUIRED gtk4)
        find_package(Threads REQUIRED)
        find_program(PKG_CONFIG_EXECUTABLE pkg-config REQUIRED)

        ## Workaround for GTK4 lib paths not being found in some environments (homebrew macos)
        execute_process(
                COMMAND ${PKG_CONFIG_EXECUTABLE} --libs-only-L gtk4
                OUTPUT_VARIABLE GTK4_LINK_PATHS_RAW
                OUTPUT_STRIP_TRAILING_WHITESPACE
                )
        message(STATUS "GTK4_LINK_PATHS_RAW: ${GTK4_LINK_PATHS_RAW}")
        string(REPLACE " " ";" GTK4_LINK_PATHS_LIST "${GTK4_LINK_PATHS_RAW}")
        string(REPLACE "-L" "" GTK4_LINK_PATHS_LIST "${GTK4_LINK_PATHS_LIST}")
        message(STATUS "GTK4_LINK_PATHS_LIST: ${GTK4_LINK_PATHS_LIST}")

        add_executable(gtk_preview
                gtk_preview.cpp
                # ../../../libausbc/src/main/jni/UVCCamera/ImageUseCases.cpp
                )
        target_link_directories(gtk_preview PRIVATE ${GTK4_LINK_PATHS_LIST})

        target_include_directories(gtk_preview PUBLIC
                ${GTK4_INCLUDE_DIRS}
                )
        target_link_libraries(
                gtk_preview
                Sources
                U8x8Font
                ${GTK4_LIBRARIES}
                ${CMAKE_THREAD_LIBS_INIT}
                )
endif()                