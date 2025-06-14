cmake_minimum_required(VERSION 3.22)

if(BUILD_SOURCES_MODULE)
        list(APPEND SOURCES 
                src/Source.cpp
                src/TestSource.cpp
                )

        if(BUILD_SOURCES_MODULE_V4L2)
                list(APPEND SOURCES 
                        src/UvcCamera.cpp)
        endif()
        
        add_library(Sources STATIC
                ${SOURCES}
                )

        target_include_directories(Sources PUBLIC
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                )                
endif()                