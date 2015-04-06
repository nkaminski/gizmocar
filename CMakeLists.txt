cmake_minimum_required(VERSION 2.8)
project( colortracking )
find_package( OpenCV REQUIRED )
add_executable( colortrack colortrack.cpp )
target_link_libraries( colortrack ${OpenCV_LIBS} )

