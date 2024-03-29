cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME LU_decomposition)
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)

# Options
set(WARNINGS_AS_ERRORS ON)
set(ENABLE_PVS_STUDIO OFF)
#! ENABLE_SANITIZERS is the option for the test builds!
#  Definitely enable it while developing.
#  Disable it for the production builds and before submitting for grading!
set(ENABLE_SANITIZERS OFF)

#! Build release version if not specified otherwise.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Include headers
include_directories(include)

# Project source compilation
add_executable(${PROJECT_NAME} src/main.cpp src/LU.cpp)
#add_executable(read_bin_mat src/read_bin_matrix.cpp)

#if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -march=native")
#endif()

add_executable(InstanceGenerator src/InstanceGenerator.cpp include/barrier.hpp)
target_link_libraries(InstanceGenerator ${CMAKE_THREAD_LIBS_INIT})

#
find_package(Threads)

# Add external packages
find_package(Boost 1.71.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} Boost::program_options ${CMAKE_THREAD_LIBS_INIT})

# set output directory (bin)
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#####################################
# Define ALL_TARGETS variable to use in PVS and Sanitizers
set(ALL_TARGETS ${PROJECT_NAME})

# Include fixed CMake configuration
include(cmake/config.cmake)
