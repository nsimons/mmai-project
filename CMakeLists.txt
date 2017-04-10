cmake_minimum_required(VERSION 2.8.12)

set(MMAI_VERSION_MAJOR 0)
set(MMAI_VERSION_MINOR 1)
set(MMAI_VERSION_FULL ${MMAI_VERSION_MAJOR}.${MMAI_VERSION_MINOR})

project(mmai-project)

set(MMAI_SRCS
    src/mmai-project.cpp)

add_executable(mmai-exec ${MMAI_SRCS})

set_target_properties(
    mmai-exec
    PROPERTIES
    VERSION ${MMAI_VERSION_FULL})
