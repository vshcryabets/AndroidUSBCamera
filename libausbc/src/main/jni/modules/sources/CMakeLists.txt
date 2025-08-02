cmake_minimum_required(VERSION 3.22)

if(BUILD_SOURCES_MODULE)

        add_library(Sources STATIC)
        target_sources(Sources PRIVATE
                src/Source.cpp
                src/TestSource.cpp
                src/TestSourceYUV420.cpp
                src/FrameDataInjectUseCase.cpp
                src/TestFileSource.cpp
                )
        if(BUILD_SOURCES_MODULE_V4L2)
                target_sources(Sources PRIVATE src/UvcCamera.cpp)
        endif()
        target_include_directories(Sources PUBLIC
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                )
        if (BUILD_CATCH2_TESTS)
                add_executable(Sources_tests)
                target_sources(Sources_tests PRIVATE
                        test/test_FrameInjector.cpp
                        test/test_FileWritter.cpp
                        )
                target_include_directories(Sources_tests PRIVATE ./include/)
                target_link_libraries(Sources_tests PRIVATE Catch2::Catch2WithMain Sources)
        endif()              
endif()                