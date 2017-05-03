cmake_minimum_required(VERSION 2.8.12)

set(MMAI_VERSION_MAJOR 0)
set(MMAI_VERSION_MINOR 1)
set(MMAI_VERSION_FULL ${MMAI_VERSION_MAJOR}.${MMAI_VERSION_MINOR})

set(CMAKE_CXX_STANDARD 11)

project(mmai-project)

find_library(SND_LIB sndfile)
if(NOT SND_LIB)
	message(FATAL_ERROR "libsndfile library not found")
endif()

find_library(PortAudio_LIB portaudio)
if(NOT PortAudio_LIB)
	message(FATAL_ERROR "portaudio library not found")
endif()

find_library(Ncurses_LIB ncurses)
if(NOT Ncurses_LIB)
    message(FATAL_ERROR "libncurses5 library not found")
endif()

include_directories(include)
	
file(GLOB MMAI_SRCS "src/*.cpp")

add_executable(mmai-exec ${MMAI_SRCS})

set_target_properties(
    mmai-exec
    PROPERTIES
    VERSION ${MMAI_VERSION_FULL})

target_link_libraries(mmai-exec sndfile portaudio pthread atomic ncurses)
