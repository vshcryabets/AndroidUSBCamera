cmake_minimum_required(VERSION 3.22)

if(BUILD_UVCCAPTURE_APP)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(GTK4 REQUIRED gtk4)
        find_package(Threads REQUIRED)

        add_executable(gtk_preview
                gtk_preview.cpp
                ../../../libausbc/src/main/jni/UVCCamera/ImageUseCases.cpp
                )

        target_include_directories(gtk_preview PUBLIC
                ${GTK4_INCLUDE_DIRS}
                )

        target_link_libraries(
                gtk_preview
                Sources
                ${GTK4_LIBRARIES}
                ${CMAKE_THREAD_LIBS_INIT}
                )
endif()                