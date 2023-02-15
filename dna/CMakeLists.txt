cmake_minimum_required(VERSION 3.8)

project(cognitiv-coding-challenge-dna)

set(CMAKE_CXX_STANDARD 20)
if (CMAKE_GNU_COMPILER_ID)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts")
endif(CMAKE_GNU_COMPILER_ID)

add_library(cogdna INTERFACE)
target_include_directories(cogdna
		INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_subdirectory(test)
