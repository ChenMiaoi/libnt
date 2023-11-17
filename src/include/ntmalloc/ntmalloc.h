#ifndef __LIBNT_MALLOC_H
#define __LIBNT_MALLOC_H

#include "defs.h"
#include <cstddef>

#define nt_export(type)         __attribute__((visibility("default"))) type
/**
 * @brief Attribute to inform the compiler that the return value of a function 
 * should be treated as a pointer to memory allocated by malloc
 */
#define nt_attr_malloc          __attribute__((malloc))
/**
 * @brief Attribute to inform the compiler that a function parameter 
 * should be treated as the size of allocated memory
 * @param position The position determining the allocated memory size
 */
#define nt_attr_alloc_s1(position)   __attribute__((alloc_size(position)))
/**
 * @brief Attribute to inform the compiler that a pair of function parameters 
 * should be treated as the sizes of allocated memory, 
 * where the total size is the product of position1 and position2
 * @param position1 The position of the first dimension of the allocated memory
 * @param position2 The position of the second dimension of the allocated memory
 */
#define nt_attr_alloc_s2(position1, position2)   __attribute__((alloc_size(position1, position2)))

NT_NAMESPACE_BEGEN

nt_export(void*) nt_malloc(size_t size) nt_attr_malloc nt_attr_alloc_s1(1);

NT_NAMESPACE_END

#endif //! __LIBNT_MALLOC_H