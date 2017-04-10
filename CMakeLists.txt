cmake_minimum_required(VERSION 2.8.12)

set(MMAI_VERSION_MAJOR 0)
set(MMAI_VERSION_MINOR 1)
set(MMAI_VERSION_FULL ${MMAI_VERSION_MAJOR}.${MMAI_VERSION_MINOR})

project(mmai-projec)

find_library(SND_LIB sndfile)
if(NOT SND_LIB)
	message(FATAL_ERROR "libsndfile library not found")
endif()


include_directories(include)
	
file(GLOB MMAI_SRCS "src/*.cpp")

add_executable(mmai-exec ${MMAI_SRCS})

set_target_properties(
    mmai-exec
    PROPERTIES
    VERSION ${MMAI_VERSION_FULL})

	
	
target_link_libraries(mmai-exec sndfile)