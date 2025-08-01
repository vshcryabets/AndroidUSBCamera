cmake_minimum_required(VERSION 3.28)

if(BUILD_DECODERS_MODULE)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(X264 REQUIRED x264)

        message(STATUS "Decoders X264_CFLAGS: ${X264_CFLAGS}")
        message(STATUS "Decoders X264_LIBRARIES: ${X264_LIBRARIES}")
        message(STATUS "DecodersX264_LIBRARY_DIRS: ${X264_LIBRARY_DIRS}")
        # message(STATUS "X264_LIBS: ${X264_LIBS}")

        add_library(Decoders STATIC)

        target_sources(Decoders PRIVATE
                src/Decoderx264.cpp
                )
        target_link_directories(Decoders PRIVATE ${X264_LIBRARY_DIRS})
        target_compile_options(Decoders PRIVATE ${X264_CFLAGS})

        target_link_libraries(Decoders PRIVATE
                ${X264_LIBRARIES}
                )

        target_include_directories(Decoders PUBLIC
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                )

        if (BUILD_CATCH2_TESTS)
                add_executable(Decoders_tests
                        test/test_Decoderx264.cpp
                        )
                target_compile_options(Decoders_tests PRIVATE ${X264_CFLAGS})
                target_include_directories(Decoders_tests PRIVATE ./include/)
                target_link_directories(Decoders_tests PRIVATE ${X264_LIBRARY_DIRS})
                target_link_libraries(Decoders_tests PRIVATE Catch2::Catch2WithMain Decoders ${X264_LIBRARIES})
        endif()
endif()                