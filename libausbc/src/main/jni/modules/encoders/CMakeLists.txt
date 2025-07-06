cmake_minimum_required(VERSION 3.28)

if(BUILD_ENCODERS_MODULE)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(X264 REQUIRED x264)

        message(STATUS "X264_CFLAGS: ${X264_CFLAGS}")
        message(STATUS "X264_LIBRARIES: ${X264_LIBRARIES}")
        message(STATUS "X264_LIBRARY_DIRS: ${X264_LIBRARY_DIRS}")
        # message(STATUS "X264_LIBS: ${X264_LIBS}")

        add_library(Encoders STATIC
                src/Encoderx264.cpp
                )

        target_link_directories(Encoders PRIVATE ${X264_LIBRARY_DIRS})
        target_compile_options(Encoders PRIVATE ${X264_CFLAGS})

        target_link_libraries(Encoders PRIVATE
                ${X264_LIBRARIES}
                )

        target_include_directories(Encoders PUBLIC
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                )

        if (BUILD_CATCH2_TESTS)
                add_executable(encoders_tests
                        test/test_Encoderx264.cpp
                        )
                target_compile_options(encoders_tests PRIVATE ${X264_CFLAGS})
                target_include_directories(encoders_tests PRIVATE ./include/)
                target_link_directories(encoders_tests PRIVATE ${X264_LIBRARY_DIRS})
                target_link_libraries(encoders_tests PRIVATE Catch2::Catch2WithMain Encoders ${X264_LIBRARIES})
        endif()
endif()                