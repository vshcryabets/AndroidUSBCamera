cmake_minimum_required(VERSION 3.22)

set(LIBJPEGTURBO ${CMAKE_CURRENT_LIST_DIR})

add_library(libJpegStatic STATIC IMPORTED GLOBAL)

set_target_properties(libJpegStatic PROPERTIES
        IMPORTED_LOCATION "${LIBJPEGTURBO}/${ANDROID_ABI}/libturbojpeg.a"
)
set_target_properties(libJpegStatic PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${LIBJPEGTURBO}/${ANDROID_ABI};${LIBJPEGTURBO}/include"
)
