cmake_minimum_required(VERSION 3.31)

if(BUILD_JNI_WRAPPERS)
    find_package(JNI REQUIRED COMPONENTS JVM)
    
    add_library(JniWrapper SHARED
        src/main.cpp
    )
    target_include_directories(
        JniWrapper
        PUBLIC
        ${JNI_INCLUDE_DIRS}
        )
    target_link_libraries(
        JniWrapper PUBLIC
        Images
        JNI::JNI
        U8x8Font
    )

    ## Force linker to include all symbols from the Sources library
    if(APPLE)
        target_link_libraries(
            JniWrapper PRIVATE
            Sources
        )
        target_link_libraries(JniWrapper PRIVATE
            "LINKER:-force_load,$<TARGET_FILE:Sources>"
        )
    else()
        target_link_libraries(JniWrapper PRIVATE "$<LINK_LIBRARY:WHOLE_ARCHIVE,Sources>")
    endif()

    # Force linker to report undefined symbols as errors during shared library creation
    if(APPLE)
        # On macOS/Apple platforms, use the ld64 linker syntax
        target_link_options(JniWrapper PRIVATE -Wl,-undefined,error)
    else()
        # On other platforms (like Linux), assume GNU ld linker syntax
        target_link_options(JniWrapper PRIVATE 
            -Wl,--no-undefined
            -Wl,--no-allow-shlib-undefined
        )
    endif()
endif()