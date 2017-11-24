cmake_minimum_required(VERSION 2.8.8)

project(zlib C)

set(zlib_srcs
	adler32.c
	compress.c
	crc32.c
	deflate.c
	gzclose.c
	gzlib.c
	gzread.c
	gzwrite.c
	infback.c
	inffast.c
	inflate.c
	inftrees.c
	trees.c
	uncompr.c
	zutil.c
)

add_library(zlibstatic ${zlib_srcs})
target_include_directories(zlibstatic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
