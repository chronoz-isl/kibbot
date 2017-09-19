cmake_minimum_required(VERSION 2.8)
project( Autobot )

set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/opencv")
set(KinectSDK_DIR "C:/Program Files/Microsoft SDKs/Kinect/v1.8")
set(Legacy_DEF_LIB "legacy_stdio_definitions.lib")

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package( Boost REQUIRED ) 
if (Boost_FOUND)
	message("[ OK ] Boost Found")
	include_directories (SYSTEM ${Boost_INCLUDE_DIR})
	link_directories (${Boost_LIBRARY_DIRS})
endif ()

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include" 

"${KinectSDK_DIR}/inc")
add_executable( Autobot _main.cpp )
target_link_libraries( Autobot ${OpenCV_LIBS} 

"${CMAKE_SOURCE_DIR}/lib/RobotConnector.lib" ${Legacy_DEF_LIB} 

"${KinectSDK_DIR}/lib/x86/Kinect10.lib" ${Boost_LIBRARIES})
