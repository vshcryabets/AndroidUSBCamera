cmake_minimum_required(VERSION 3.22)

set(DIR_HACKS ${CMAKE_CURRENT_LIST_DIR}/)
include_directories(
    ${DIR_HACKS}/
)

list(APPEND SOURCES
	${DIR_HACKS}/uvchacks.cpp
    )
