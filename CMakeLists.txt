cmake_minimum_required(VERSION 2.8)
project( cbr-vsss )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} arduino-serial )
add_executable( 
	cbr-vsss main.cpp
	arduino-serial-lib.c
)
target_link_libraries( cbr-vsss ${OpenCV_LIBS} )