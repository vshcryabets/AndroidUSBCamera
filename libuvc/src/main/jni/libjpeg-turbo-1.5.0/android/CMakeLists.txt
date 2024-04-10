cmake_minimum_required(VERSION 3.10.2)

set(LIBJPEGTURBO ${CMAKE_CURRENT_LIST_DIR}/..)

include_directories(
	${LIBJPEGTURBO}/
	${LIBJPEGTURBO}/include/
)

list(APPEND SOURCES
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
