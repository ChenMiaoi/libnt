# Setting the directory for ntmalloc
set(ntmalloc_dir "src/include/ntmalloc")

# Globbing all .cc files in the src directory for ntmalloc sources
file(GLOB ntmalloc_sources src/*.cc)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  list(APPEND ntmalloc_flags -Wall -Wno-unknown-pragmas -ftls-model=initial-exec)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    list(APPEND ntmalloc_flags -Wno-invalid-memory-model) # Appending specific flag for GNU compiler
    list(APPEND ntmalloc_flags -fvisibility=hidden)       # Appending visibility flag when create share lib
  endif()
endif()

set(ntmalloc_basename "ntmalloc")
message(STATUS "Output library name   : ${ntmalloc_basename}")

# Create a static library target named ntmalloc_static with the specified sources
add_library(ntmalloc_static STATIC ${ntmalloc_sources})
set_target_properties(ntmalloc_static PROPERTIES OUTPUT_NAME ${ntmalloc_basename})
target_compile_definitions(ntmalloc_static PRIVATE )
target_compile_options(ntmalloc_static PRIVATE ${ntmalloc_flags})
target_include_directories(ntmalloc_static PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}${ntmalloc_dir}>
)