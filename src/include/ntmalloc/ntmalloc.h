/**
 * @file ntmalloc.h
 * @brief This is the ntmalloc library, a memory allocation library based on Microsoft's mimalloc.
 * 
 * This library is derived from mimalloc, a compact and efficient allocator for C and C++.
 * For more information about mimalloc, please visit: https://github.com/microsoft/mimalloc
 * 
 * The ntmalloc library includes modifications and adaptations based on mimalloc to suit specific requirements and use cases.
 * These changes are aimed at enhancing performance, adding new features, or addressing specific needs in the context of the ntmalloc library.
 * 
 * For detailed information about the modifications made to mimalloc in the context of ntmalloc, please refer to the source code and documentation.
 */

#ifndef __LIBNT_MALLOC_H
#define __LIBNT_MALLOC_H

#include "../defs.h"
#include "types.h"

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

nt_export(nt_heap_t*) nt_heap_new(void);

nt_export(void*) nt_heap_malloc(nt_heap_t* heap, size_t size) nt_attr_malloc nt_attr_alloc_s1(2);

NT_NAMESPACE_END

#endif //! __LIBNT_MALLOC_H