cmake_minimum_required(VERSION 3.22)

set(LIBJPEGTURBO ${CMAKE_CURRENT_LIST_DIR})

include_directories(
        ${LIBJPEGTURBO}/include
        ${LIBJPEGTURBO}/${ANDROID_ABI}
)

add_library(libJpegStatic STATIC IMPORTED)
set_target_properties(libJpegStatic PROPERTIES IMPORTED_LOCATION ${LIBJPEGTURBO}/${ANDROID_ABI}/libturbojpeg.a)

#target_include_directories(libJpegStatic INTERFACE
#        $<BUILD_INTERFACE:${LIBJPEGTURBO}/include>
#        $<BUILD_INTERFACE:${LIBJPEGTURBO}/${ANDROID_ABI}>
#)

