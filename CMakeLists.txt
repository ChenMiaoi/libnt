cmake_minimum_required(VERSION 3.2.0)
project(libnt)

# Set the C++ language standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Generate an executable file
add_executable(main main.cc)

# Generate a file descriptor static library
add_library(fd src/fd.cc)
target_include_directories(fd PUBLIC src/include)

# Add a test subdirectory
add_subdirectory(tests)

# Add gtest test framework dependencies
# First look in the local /usr/local/include, otherwise download directly
find_path(GTEST_INCLUDE_DIR gtest PATHS /usr/local/include)
if(NOT GTEST_INCLUDE_DIR)
  # Not find gtest in the local
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.1.tar.gz
  )
  FetchContent_MakeAvailable(googletest)
else()
  # Add locally
  include_directories(/usr/local/include/gtest)
  link_directories(/usr/local/lib)
endif()

target_link_libraries(main gtest)
