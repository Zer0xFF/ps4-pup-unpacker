cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ps4-pup_unpacker CXX)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/zlib-1.2.8)

set(SOURCE
	main.cpp
	PUP.cpp
	MainApp.cpp
)

set(HEADERS
	BinaryReader.h
	Entry.h
	PUP.h
	MainApp.h
)

add_executable(pup_unpacker ${SOURCE} ${HEADERS})
target_link_libraries(pup_unpacker PRIVATE zlibstatic)
target_include_directories(pup_unpacker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extern/zstr/src/)
