cmake_minimum_required(VERSION 3.22)

set(COMMON ${CMAKE_CURRENT_LIST_DIR}/)
include_directories(
    ${COMMON}/
)

list(APPEND SOURCES
	${COMMON}/utilbase.cpp
    )
