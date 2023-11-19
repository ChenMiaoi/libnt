cmake_minimum_required(VERSION 3.2.0)
project(libnt)

# Set the C++ language standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_CXX_COMPLIER_ID MATCHES "Clang|GNU")
  list(APPEND nt_flags -std=c++11 -pedantic
    -Wall -Wextra -Werror -Wstrict-prototypes -Wwrite-strings
    -Wshadow -Winit-self -Wcast-align -Wformat=2
      -Wmissing-prototypes
      -Wstrict-overflow=2
      -Wcast-qual
      -Wundef
      -Wswitch-default
      -Wconversion
      -Wc++-compat
      -fstack-protector-strong
      -Wcomma
      -Wdouble-promotion
      -Wparentheses
      -Wformat-overflow
      -Wunused-macros
      -Wmissing-variable-declarations
      -Wused-but-marked-unused
      -Wswitch-enum
  )
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    list(APPEND nt_flags -Wno-invalid-memory-model)
    list(APPEND nt_flags -fvisibility=hidden)
  endif()
endif()

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

add_subdirectory(src/include/ntmalloc)

target_link_libraries(main fd ntmalloc_static gtest)
