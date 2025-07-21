cmake_minimum_required(VERSION 3.22)

if(BUILD_JNI_LIB)
        add_library(JniWrapper SHARED
                src/main.cpp
        )

        target_link_libraries(
                JniWrapper
                Sources
                Images
                U8x8Font
                )
endif()                