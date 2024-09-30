cmake_minimum_required(VERSION 3.14)

# Project name and version
project(OptionsKillerBotCPP VERSION 1.0 LANGUAGES CXX)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set compiler flags (for clang++)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -march=native -flto -fomit-frame-pointer -ffast-math")
endif()

# Set source and header directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Find libraries from vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Find dependencies (replace curl with any other dependencies)
find_package(CURL REQUIRED)

# Include directories
include_directories(${INCLUDE_DIR})

# Create a list of all source files
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# Create executable
add_executable(OptionsKillerBotCPP ${SOURCES})

# Link the required libraries
target_link_libraries(OptionsKillerBotCPP PRIVATE CURL::libcurl)

# Set up the output directory
set_target_properties(OptionsKillerBotCPP PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
