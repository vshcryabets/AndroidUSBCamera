cmake_minimum_required(VERSION 3.22)

set(UVCCAMERA ${CMAKE_CURRENT_LIST_DIR}/)
include_directories(
    ${UVCCAMERA}/
)

list(APPEND SOURCES
	${UVCCAMERA}/_onload.cpp
    ${UVCCAMERA}/UVCCamera.cpp
    ${UVCCAMERA}/UVCPreviewBase.cpp
    ${UVCCAMERA}/UVCPreviewJni.cpp
    ${UVCCAMERA}/UVCCameraAdjustments.cpp
    ${UVCCAMERA}/JniUVCCamera.cpp
    )
