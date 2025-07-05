cmake_minimum_required(VERSION 3.28)

if(BUILD_X264_TEST_APP)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(X264 REQUIRED x264)

        message(STATUS "X264_CFLAGS: ${X264_CFLAGS}")
        message(STATUS "X264_LIBRARIES: ${X264_LIBRARIES}")
        message(STATUS "X264_LIBRARY_DIRS: ${X264_LIBRARY_DIRS}")
        # message(STATUS "X264_LIBS: ${X264_LIBS}")

        add_executable(x264testApp
                src/main.cpp
                )

        target_link_directories(x264testApp PRIVATE ${X264_LIBRARY_DIRS})
        target_compile_options(x264testApp PRIVATE ${X264_CFLAGS})

        target_link_libraries(x264testApp PRIVATE
                ${X264_LIBRARIES}
                # libx264
                )
endif()                