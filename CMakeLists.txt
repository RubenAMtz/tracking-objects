cmake_minimum_required(VERSION 3.0.0)

project(trackingObjects)

# The line below works giving your opencv path
#find_package( OpenCV REQUIRED PATHS C:/opencv/build/install )

# If you have OpenCV_DIR as an Environment Variable use the next line
find_package( OpenCV REQUIRED )

set(	NAME_SRC
	src/Source.cpp
	src/helpers.cpp
)

include_directories( ${OpenCV_INCLUDE_DIRS} )

ADD_EXECUTABLE( trackingObjects ${NAME_SRC} )

TARGET_LINK_LIBRARIES( trackingObjects ${OpenCV_LIBS} )