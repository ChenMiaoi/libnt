#include "../ntmalloc.h"
#include "../ntmalloc_internal.h"

#include <cstddef>

NT_NAMESPACE_BEGEN

void* nt_heap_malloc_small(nt_heap_t* heap, size_t size) {
    nt_assert(size <= NT_SMALL_SIZE_MAX);
    nt_page_t* page = _nt_heap_get_free_small_page(heap, size);
    
    // TODO
    // return _nt_page_malloc(heap, page, size);
    return {};
}

void* nt_heap_malloc(nt_heap_t* heap, size_t size) {
    nt_assert(heap != nullptr);
    nt_assert(heap->thread_id == 0 || heap->thread_id == _nt_thread_id());

    void* p;
    if (nt_likely(size < NT_SMALL_SIZE_MAX)) {
        p = nt_heap_malloc_small(heap, size);
    } else {
        // p = _nt_malloc_generic(heap, size);
    }
    return {};
}

void* nt_malloc(size_t size) {
    return nt_heap_malloc(nt_get_default_heap(), size);
}

NT_NAMESPACE_END