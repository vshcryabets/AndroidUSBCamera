cmake_minimum_required(VERSION 3.22)

if(BUILD_JPEGBENCHMARK_APP)
        find_package( Threads REQUIRED )
        find_package (JPEG REQUIRED)

        #set(USE_TURBOJPEG 1)
        set(USE_LIBJPEG 1)

        add_executable(jpegbenchmark
                jpegdecodebenchmark.cpp
                ../../../libausbc/src/main/jni/UVCCamera/ImageUseCases.cpp
                ../../../libausbc/src/main/jni/UVCCamera/JpegBenchmark.cpp
                ../../../libausbc/src/main/jni/UVCCamera/ProgressObserver.cpp
                )

        if(USE_TURBOJPEG)
                target_compile_definitions(jpegbenchmark PRIVATE -DUSE_TURBOJPEG)
                add_library(turbojpeg SHARED IMPORTED)
                set_target_properties(turbojpeg PROPERTIES
                        IMPORTED_LOCATION /usr/lib/libturbojpeg.so)
                target_link_libraries(
                        jpegbenchmark
                        turbojpeg
                        ${CMAKE_THREAD_LIBS_INIT})         
        endif()
        
        if (USE_LIBJPEG)
                target_compile_definitions(jpegbenchmark PRIVATE -DUSE_LIBJPEG)
                target_link_libraries(
                        jpegbenchmark
                        ${JPEG_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT})        
        endif()                
endif()                