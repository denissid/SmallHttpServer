cmake_minimum_required(VERSION 3.5)

project(small_http)

set (CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_BINRY_DIR})
set (LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set (SOURCES_FILES_MAIN main.cpp)

set (SOURCES_FILES_TEST mainTests.cpp)

#set (SOURCES_FILES  Options.h Options.cpp Server.h Server.cpp ClientSocket.h ClientSocket.cpp
#    HTTPPacket.h HTTPPacket.cpp DaemonHelper.h Worker.h Worker.cpp ThreadSafeStack.h ThreadSafeStack.cpp Logger.h Exceptions.h nlohmann/json.hpp Logger.cpp)


set (SOURCES_FILES  Options.cpp Server.cpp  ClientSocket.cpp
    HTTPPacket.cpp  Worker.cpp  ThreadSafeStack.cpp nlohmann/json.hpp Logger.cpp ServerSocket.cpp)

set (CMAKE_CXX_FLAGS "-std=c++17 -pthread")

find_package(OpenSSL REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}")

add_executable (final ${PROJECT_SOURCE_DIR}/${SOURCES_FILES} ${PROJECT_SOURCE_DIR}/${SOURCES_FILES_MAIN})
target_link_libraries(final OpenSSL::SSL)
target_link_libraries (final OpenSSL::Crypto)

add_executable (tests ${PROJECT_SOURCE_DIR}/${SOURCES_FILES} ${PROJECT_SOURCE_DIR}/${SOURCES_FILES_TEST} )
target_link_libraries(tests OpenSSL::SSL)
target_link_libraries (tests OpenSSL::Crypto)
