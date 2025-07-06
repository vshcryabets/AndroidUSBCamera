cmake_minimum_required(VERSION 3.28)

if(BUILD_X264_TEST_APP)
        add_executable(x264testApp
                src/main.cpp
                )

        target_link_directories(x264testApp PRIVATE ${X264_LIBRARY_DIRS})
        target_compile_options(x264testApp PRIVATE ${X264_CFLAGS})

        target_link_libraries(x264testApp PRIVATE
                Encoders
                )

        target_include_directories(x264testApp PUBLIC
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                )
endif()                