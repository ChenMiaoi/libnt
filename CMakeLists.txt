cmake_minimum_required(VERSION 3.2.0)
project(libnt)

# Set the C++ language standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_CXX_COMPLIER_ID MATCHES "Clang|GNU")
  list(APPEND nt_flags -Wall -Wextra -Wno-unknown-pragmas -ftls-model=initial-exec)
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

set(ntmalloc_dir "src/include/ntmalloc")
file(GLOB ntmalloc_sources ${ntmalloc_dir}/src/*.cc)

set(ntmalloc_basename "ntmalloc")
message(STATUS "Output library name   : ${ntmalloc_basename}")

add_library(ntmalloc_static STATIC ${ntmalloc_sources})
set_target_properties(ntmalloc_static PROPERTIES OUTPUT_NAME ${ntmalloc_basename})
# target_compile_definitions(ntmalloc_static PRIVATE )
target_compile_options(ntmalloc_static PRIVATE ${nt_flags})
target_include_directories(ntmalloc_static PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}${ntmalloc_dir}/src>
)

target_link_libraries(main gtest ntmalloc_static)
