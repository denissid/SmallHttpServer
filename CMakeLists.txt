cmake_minimum_required(VERSION 2.8)

project(small_http)

set (CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_BINRY_DIR})
set (LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set (SOURCES_FILES main.cpp Options.h Options.cpp Server.h Server.cpp ClientSocket.h ClientSocket.cpp
		   HTTPPacket.h HTTPPacket.cpp DaemonHelper.h Worker.h )

set (CMAKE_CXX_FLAGS "-std=c++11")

include_directories("${PROJECT_SOURCE_DIR}")

add_executable (final ${PROJECT_SOURCE_DIR}/${SOURCES_FILES})
