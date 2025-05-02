cmake_minimum_required(VERSION 3.22)

set(LIBJPEGTURBO ${CMAKE_CURRENT_LIST_DIR}/)

include_directories(
	${LIBJPEGTURBO}/${ANDROID_ABI}/
	${LIBJPEGTURBO}/include/
)

add_library(jpeg-static STATIC IMPORTED )
set_target_properties(jpeg-static PROPERTIES IMPORTED_LOCATION ${LIBJPEGTURBO}/${ANDROID_ABI}/libturbojpeg.a)