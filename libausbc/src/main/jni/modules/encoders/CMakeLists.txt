cmake_minimum_required(VERSION 3.28)

find_package(PkgConfig REQUIRED)
pkg_check_modules(X264 REQUIRED x264)

message(STATUS "Encoders X264_CFLAGS: ${X264_CFLAGS}")
message(STATUS "Encoders X264_LIBRARIES: ${X264_LIBRARIES}")
message(STATUS "Encoders X264_LIBRARY_DIRS: ${X264_LIBRARY_DIRS}")
# message(STATUS "X264_LIBS: ${X264_LIBS}")

add_library(Encoders STATIC)
target_sources(Encoders PRIVATE
        src/Encoderx264.cpp
        )
target_link_directories(Encoders PRIVATE ${X264_LIBRARY_DIRS})
target_compile_options(Encoders PRIVATE ${X264_CFLAGS})

target_link_libraries(Encoders PRIVATE
        ${X264_LIBRARIES}
        Sources
        )

target_include_directories(Encoders PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

if (BUILD_CATCH2_TESTS)
        add_executable(Encoders_tests
                test/test_Encoderx264.cpp
                )
        target_compile_options(Encoders_tests PRIVATE ${X264_CFLAGS})
        target_include_directories(Encoders_tests PRIVATE ./include/)
        target_link_directories(Encoders_tests PRIVATE ${X264_LIBRARY_DIRS})
        target_link_libraries(Encoders_tests PRIVATE
            Catch2::Catch2WithMain 
            Encoders 
            Sources
            U8x8Font
            )

        catch_discover_tests(Encoders_tests
            PROPERTIES
            TIMEOUT 10)
endif()
