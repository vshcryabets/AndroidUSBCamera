cmake_minimum_required(VERSION 3.18)

set(LIBJPEGTURBO ${CMAKE_CURRENT_LIST_DIR}/..)

include_directories(
	${LIBJPEGTURBO}/
	${LIBJPEGTURBO}/include/
)

enable_language(ASM)

set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_ASM_FLAGS}")

message(${ANDROID_ABI})
if (ANDROID_ABI STREQUAL "arm64-v8a")
	set(CPU_TYPE arm64)
elseif (ANDROID_ABI STREQUAL "armeabi-v7a")
	set(CPU_TYPE arm)
elseif (ANDROID_ABI STREQUAL "x86_64")
	set(CPU_TYPE x86_64)
elseif (ANDROID_ABI STREQUAL "x86")
	set(CPU_TYPE x86)
endif ()

if(CPU_TYPE STREQUAL "x86_64" OR CPU_TYPE STREQUAL "x86")
	set(CMAKE_ASM_NASM_FLAGS_DEBUG_INIT "-g")
	set(CMAKE_ASM_NASM_FLAGS_RELWITHDEBINFO_INIT "-g")
endif()

#
#if(CPU_TYPE STREQUAL "arm64")
#	### ARM64 + NEON
#	add_compile_definitions(SIZEOF_SIZE_T=8)
#	list(APPEND SIMD_SOURCES
#			${LIBJPEGTURBO}/simd/jsimd_arm64_neon.S
#			${LIBJPEGTURBO}/simd/jsimd_arm64.c
#	)
#elseif(CPU_TYPE STREQUAL "arm")
#	## ARM32 + NEON
#	add_compile_definitions(SIZEOF_SIZE_T=4)
#	list(APPEND SIMD_SOURCES
#			${LIBJPEGTURBO}/simd/jsimd_arm_neon.S
#			${LIBJPEGTURBO}/simd/jsimd_arm.c
#	)
#elseif(CPU_TYPE STREQUAL "x86")
#	## x86
#	add_compile_definitions(SIZEOF_SIZE_T=4)
#	list(APPEND SIMD_SOURCES
#		${LIBJPEGTURBO}/simd/jsimd_i386.c
#		${LIBJPEGTURBO}/simd/jsimdcpu.asm
#		${LIBJPEGTURBO}/simd/jfdctflt-3dn.asm
#		${LIBJPEGTURBO}/simd/jidctflt-3dn.asm
#		${LIBJPEGTURBO}/simd/jquant-3dn.asm
#		${LIBJPEGTURBO}/simd/jccolor-mmx.asm
#		${LIBJPEGTURBO}/simd/jcgray-mmx.asm
#		${LIBJPEGTURBO}/simd/jcsample-mmx.asm
#		${LIBJPEGTURBO}/simd/jdcolor-mmx.asm
#		${LIBJPEGTURBO}/simd/jdmerge-mmx.asm
#		${LIBJPEGTURBO}/simd/jdsample-mmx.asm
#		${LIBJPEGTURBO}/simd/jfdctfst-mmx.asm
#		${LIBJPEGTURBO}/simd/jfdctint-mmx.asm
#		${LIBJPEGTURBO}/simd/jidctfst-mmx.asm
#		${LIBJPEGTURBO}/simd/jidctint-mmx.asm
#		${LIBJPEGTURBO}/simd/jidctred-mmx.asm
#		${LIBJPEGTURBO}/simd/jquant-mmx.asm
#		${LIBJPEGTURBO}/simd/jfdctflt-sse.asm
#		${LIBJPEGTURBO}/simd/jidctflt-sse.asm
#		${LIBJPEGTURBO}/simd/jquant-sse.asm
#		${LIBJPEGTURBO}/simd/jccolor-sse2.asm
#		${LIBJPEGTURBO}/simd/jcgray-sse2.asm
#		${LIBJPEGTURBO}/simd/jcsample-sse2.asm
#		${LIBJPEGTURBO}/simd/jdcolor-sse2.asm
#		${LIBJPEGTURBO}/simd/jdmerge-sse2.asm
#		${LIBJPEGTURBO}/simd/jdsample-sse2.asm
#		${LIBJPEGTURBO}/simd/jfdctfst-sse2.asm
#		${LIBJPEGTURBO}/simd/jfdctint-sse2.asm
#		${LIBJPEGTURBO}/simd/jidctflt-sse2.asm
#		${LIBJPEGTURBO}/simd/jidctfst-sse2.asm
#		${LIBJPEGTURBO}/simd/jidctint-sse2.asm
#		${LIBJPEGTURBO}/simd/jidctred-sse2.asm
#		${LIBJPEGTURBO}/simd/jquantf-sse2.asm
#		${LIBJPEGTURBO}/simd/jquanti-sse2.asm
#		${LIBJPEGTURBO}/simd/jchuff-sse2.asm
#	)
#elseif (CPU_TYPE STREQUAL "x86_64")
#	## x86_64
#	add_compile_definitions(SIZEOF_SIZE_T=8)
#	list(APPEND SIMD_SOURCES
#		${LIBJPEGTURBO}/simd/jsimd_x86_64.c
#		${LIBJPEGTURBO}/simd/jfdctflt-sse-64.asm
#		${LIBJPEGTURBO}/simd/jccolor-sse2-64.asm
#		${LIBJPEGTURBO}/simd/jcgray-sse2-64.asm
#		${LIBJPEGTURBO}/simd/jcsample-sse2-64.asm
#		${LIBJPEGTURBO}/simd/jdcolor-sse2-64.asm
#		${LIBJPEGTURBO}/simd/jdmerge-sse2-64.asm
#		${LIBJPEGTURBO}/simd/jdsample-sse2-64.asm
#		${LIBJPEGTURBO}/simd/jfdctfst-sse2-64.asm
#		${LIBJPEGTURBO}/simd/jfdctint-sse2-64.asm
#		${LIBJPEGTURBO}/simd/jidctflt-sse2-64.asm
#		${LIBJPEGTURBO}/simd/jidctfst-sse2-64.asm
#		${LIBJPEGTURBO}/simd/jidctint-sse2-64.asm
#		${LIBJPEGTURBO}/simd/jidctred-sse2-64.asm
#		${LIBJPEGTURBO}/simd/jquantf-sse2-64.asm
#		${LIBJPEGTURBO}/simd/jquanti-sse2-64.asm
#		${LIBJPEGTURBO}/simd/jchuff-sse2-64.asm
#	)
#endif()
#
#add_library(
#		simd OBJECT ${SIMD_SOURCES}
#)

add_subdirectory(${LIBJPEGTURBO}/simd)

list(APPEND JPEG_SOURCES
    ${LIBJPEGTURBO}/jcapimin.c
	${LIBJPEGTURBO}/jcapistd.c
	${LIBJPEGTURBO}/jccoefct.c
	${LIBJPEGTURBO}/jccolor.c
	${LIBJPEGTURBO}/jcdctmgr.c
	${LIBJPEGTURBO}/jchuff.c
	${LIBJPEGTURBO}/jcinit.c
	${LIBJPEGTURBO}/jcmainct.c
	${LIBJPEGTURBO}/jcmarker.c
	${LIBJPEGTURBO}/jcmaster.c
	${LIBJPEGTURBO}/jcomapi.c
	${LIBJPEGTURBO}/jcparam.c
	${LIBJPEGTURBO}/jcphuff.c
	${LIBJPEGTURBO}/jcprepct.c
	${LIBJPEGTURBO}/jcsample.c
	${LIBJPEGTURBO}/jctrans.c
	${LIBJPEGTURBO}/jdapimin.c
	${LIBJPEGTURBO}/jdapistd.c
	${LIBJPEGTURBO}/jdatadst.c
	${LIBJPEGTURBO}/jdatasrc.c
	${LIBJPEGTURBO}/jdcoefct.c
	${LIBJPEGTURBO}/jdcolor.c
	${LIBJPEGTURBO}/jddctmgr.c
	${LIBJPEGTURBO}/jdhuff.c
	${LIBJPEGTURBO}/jdinput.c
	${LIBJPEGTURBO}/jdmainct.c
	${LIBJPEGTURBO}/jdmarker.c
	${LIBJPEGTURBO}/jdmaster.c
	${LIBJPEGTURBO}/jdmerge.c
	${LIBJPEGTURBO}/jdphuff.c
	${LIBJPEGTURBO}/jdpostct.c
	${LIBJPEGTURBO}/jdsample.c
	${LIBJPEGTURBO}/jdtrans.c
	${LIBJPEGTURBO}/jerror.c
	${LIBJPEGTURBO}/jfdctflt.c
	${LIBJPEGTURBO}/jfdctfst.c
	${LIBJPEGTURBO}/jfdctint.c
	${LIBJPEGTURBO}/jidctflt.c
	${LIBJPEGTURBO}/jidctfst.c
	${LIBJPEGTURBO}/jidctint.c
	${LIBJPEGTURBO}/jidctred.c
	${LIBJPEGTURBO}/jquant1.c
	${LIBJPEGTURBO}/jquant2.c
	${LIBJPEGTURBO}/jutils.c
	${LIBJPEGTURBO}/jmemmgr.c
	${LIBJPEGTURBO}/jmemnobs.c
    ${LIBJPEGTURBO}/jaricom.c
	${LIBJPEGTURBO}/jcarith.c
	${LIBJPEGTURBO}/jdarith.c
	${LIBJPEGTURBO}/turbojpeg.c
	${LIBJPEGTURBO}/transupp.c
	${LIBJPEGTURBO}/jdatadst-tj.c
	${LIBJPEGTURBO}/jdatasrc-tj.c
    )

add_library(
		jpeg-static ${JPEG_SOURCES} $<TARGET_OBJECTS:simd>
)

