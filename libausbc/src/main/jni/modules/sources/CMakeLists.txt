cmake_minimum_required(VERSION 3.22)

add_library(Sources STATIC)
set_target_properties(Sources PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_sources(Sources PRIVATE
        src/Source.cpp
        src/TestSource.cpp
        src/TestSourceYUV420.cpp
        src/FrameDataInjectUseCase.cpp
        src/TestFileSource.cpp
        src/PullToPushSource.cpp
        src/DataTypes.cpp
        )
if(BUILD_SOURCES_MODULE_V4L2)
    target_sources(Sources PRIVATE
        src/UvcCamera.cpp)
endif()

if(BUILD_JNI_WRAPPERS)
    if (NOT (CMAKE_SYSTEM_NAME STREQUAL "Android"))
        find_package(JNI REQUIRED COMPONENTS JVM)
    endif()
    target_sources(Sources PRIVATE
        src/jni/JniTestSource.cpp
        src/jni/JniSources.cpp
        src/jni/JniPullToPushSource.cpp
        )
    target_include_directories(
        Sources
        PUBLIC
        ${JNI_INCLUDE_DIRS}
        )
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
                test/test_TestFileSource.cpp
                test/test_TestSourceYUV420.cpp
                test/test_PullToPushSource.cpp
                test/test_Frame.cpp
                )
        target_compile_options(Sources_tests PRIVATE -g)
        target_include_directories(Sources_tests PRIVATE ./include/)
        target_link_libraries(Sources_tests PRIVATE Catch2::Catch2WithMain Sources U8x8Font)

        set(RESOURCE_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/test/testh264.bin"
        )
        add_custom_command(
            TARGET Sources_tests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${RESOURCE_FILES}
                $<TARGET_FILE_DIR:Sources_tests>
            COMMENT "Copying resources to build directory..."
        )

        catch_discover_tests(Sources_tests
            PROPERTIES
            TIMEOUT 10)
endif()              
