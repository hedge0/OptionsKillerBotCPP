cmake_minimum_required(VERSION 3.14)

# Set the vcpkg toolchain file path
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Project name and version
project(OptionsKillerBotCPP VERSION 1.0 LANGUAGES CXX)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set compiler flags (for clang++)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -march=native -flto -fomit-frame-pointer -ffast-math")
endif()

include(FetchContent)

FetchContent_Declare(
  jsonifier
  GIT_REPOSITORY https://github.com/realtimechris/jsonifier.git
  GIT_TAG dev
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(jsonifier)

# Set source and header directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Find the curl package using vcpkg
find_package(CURL REQUIRED)

find_package(OpenSSL REQUIRED)

find_package(Eigen3 CONFIG REQUIRED)

# Include directories
include_directories(${INCLUDE_DIR})

# Create a list of all source files
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# Create executable
add_executable(OptionsKillerBotCPP ${SOURCES})

# Link the required libraries
target_link_libraries(OptionsKillerBotCPP PRIVATE CURL::libcurl Eigen3::Eigen Jsonifier::Jsonifier OpenSSL::SSL OpenSSL::Crypto)

# Set up the output directory
set_target_properties(OptionsKillerBotCPP PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
