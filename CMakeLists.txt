cmake_minimum_required(VERSION 2.8)
project( colortrack )
find_package( OpenCV REQUIRED )
add_executable( colortrack comm.cpp colortrack.cpp base64.c crc16.c serio.c )
target_link_libraries( colortrack ${OpenCV_LIBS} pthread )
set( CMAKE_CXX_FLAGS -std=c++11 )
