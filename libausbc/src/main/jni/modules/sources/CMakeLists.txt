cmake_minimum_required(VERSION 3.22)

if(BUILD_SOURCES_MODULE)
        list(APPEND SOURCES 
                src/Source.cpp
                src/TestSource.cpp
                src/TestSourceYUV420.cpp
                src/FrameDataInjectUseCase.cpp
                )

        if(BUILD_SOURCES_MODULE_V4L2)
                list(APPEND SOURCES 
                        src/UvcCamera.cpp)
        endif()

        if(BUILD_JNI_WRAPPERS)
                list(APPEND SOURCES
                        src/JniTestSource.cpp
                        src/JniMapper.cpp
                        )
        endif()
        
        add_library(Sources STATIC
                ${SOURCES}
                )

        target_include_directories(Sources PUBLIC
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                )
        if (BUILD_CATCH2_TESTS)
                add_executable(Sources_tests
                        test/test_FrameInjector.cpp
                        )
                target_include_directories(Sources_tests PRIVATE ./include/)
                target_link_libraries(Sources_tests PRIVATE Catch2::Catch2WithMain Sources)
        endif()              
endif()                