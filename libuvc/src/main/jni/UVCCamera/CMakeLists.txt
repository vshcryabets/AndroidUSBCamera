cmake_minimum_required(VERSION 3.10.2)

set(UVCCAMERA ${CMAKE_CURRENT_LIST_DIR}/)
include_directories(
    $UVCCAMERA/
)

list(APPENS SOURCES
	${UVCCAMERA}/_onload.cpp
    ${UVCCAMERA}/utilbase.cpp
    ${UVCCAMERA}/UVCCamera.cpp
    ${UVCCAMERA}/UVCPreviewBase.cpp
    ${UVCCAMERA}/UVCPreviewJni.cpp
    ${UVCCAMERA}/UVCButtonCallback.cpp
    ${UVCCAMERA}/UVCStatusCallback.cpp
    ${UVCCAMERA}/UVCCameraAdjustments.cpp
    ${UVCCAMERA}/serenegiant_usb_UVCCamera.cpp
    )
