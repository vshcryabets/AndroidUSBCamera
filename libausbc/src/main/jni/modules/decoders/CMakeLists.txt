cmake_minimum_required(VERSION 3.28)

if(BUILD_DECODERS_MODULE)
        ## sudo apt-get install libavcodec-dev libavformat-dev libavutil-dev pkg-config
        ## brew install ffmpeg pkg-config
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavcodec libavformat libavutil)

        message(STATUS "Decoders FFMPEG_CFLAGS: ${FFMPEG_CFLAGS}")
        message(STATUS "Decoders FFMPEG_LIBRARIES: ${FFMPEG_LIBRARIES}")
        message(STATUS "Decoders FFMPEG_LIBRARY_DIRS: ${FFMPEG_LIBRARY_DIRS}")

        add_library(Decoders STATIC)

        target_sources(Decoders PRIVATE
                src/DecoderH264LibAVCodec.cpp
                )
        target_link_directories(Decoders PRIVATE ${FFMPEG_LIBRARY_DIRS})
        target_compile_options(Decoders PRIVATE ${FFMPEG_CFLAGS})

        target_link_libraries(Decoders PRIVATE
                PkgConfig::FFMPEG
                )

        target_include_directories(Decoders PUBLIC
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                )

        if (BUILD_CATCH2_TESTS)
                add_executable(Decoders_tests
                        test/test_DecoderH264LibAVCodec.cpp
                        )
                target_compile_options(Decoders_tests PRIVATE ${FFMPEG_CFLAGS})
                target_include_directories(Decoders_tests PRIVATE ./include/)
                # target_link_directories(Decoders_tests PRIVATE ${X264_LIBRARY_DIRS})
                target_link_libraries(Decoders_tests PRIVATE 
                        Catch2::Catch2WithMain 
                        Decoders 
                        PkgConfig::FFMPEG
                        Sources)
                set(RESOURCE_FILES
                    "${CMAKE_CURRENT_SOURCE_DIR}/test/framesFile.h264"
                )
                add_custom_command(
                    TARGET Decoders_tests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                        ${RESOURCE_FILES}
                        $<TARGET_FILE_DIR:Decoders_tests>
                    COMMENT "Copying resources to build directory..."
                )

        endif()
endif()                