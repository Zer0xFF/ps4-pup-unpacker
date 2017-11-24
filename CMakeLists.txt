cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ps4-pup_unpacker CXX)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/zlib-1.2.8)

if(UNIX AND NOT APPLE)
	unset(CPPLIBFS CACHE)
	find_library(CPPLIBFS NAMES stdc++fs c++experimental
		PATHS
			/usr/lib/
			/usr/lib64/
			/usr/local/lib64/
			/usr/lib/x86_64-linux-gnu/
			/usr/lib/gcc/x86_64-linux-gnu/5
			/usr/lib/gcc/x86_64-linux-gnu/6
	)
endif()

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
target_link_libraries(pup_unpacker PRIVATE zlibstatic ${CPPLIBFS})
target_include_directories(pup_unpacker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extern/zstr/src/)
