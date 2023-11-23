#include "../ntmalloc.h"
#include "../ntmalloc_internal.h"

#include <cstddef>
#include <string>

extern std::string stack_path;  //! In order to more intuitively reflect the function call stack

NT_NAMESPACE_BEGEN

void* _nt_page_malloc(nt_heap_t* heap, nt_page_t* page, size_t size) {
    stack_path += "_nt_page_malloc -> ";
    nt_assert_internal(page->block_size == 0 || page->block_size >= size);
    //! 获取page中的空闲链表块
    nt_block_t* block = page->free;
    info << "block == null is " << (block == nullptr);
    if (nt_unlikely(block == nullptr)) {
        //! In this case, the block must be empty because of the page
        //! 第一次会进入到一个默认的申请空间
        info << "try to malloc generic";
        return _nt_malloc_generic(heap, size);
    }
    nt_assert_internal(block != nullptr && _nt_ptr_page(block) == page);
    // pop from the free list
    
    return {};
}

void* nt_heap_malloc_small(nt_heap_t* heap, size_t size) {
    stack_path += "nt_heap_malloc_small -> ";
    nt_assert(size <= NT_SMALL_SIZE_MAX);

    //! 这里就获取到了heap中一块空闲的，默认初始化为空的page
    nt_page_t* page = _nt_heap_get_free_small_page(heap, size);
    
    //! 因此接下来就应该为page申请一块空间
    return _nt_page_malloc(heap, page, size);
}

void* nt_heap_malloc(nt_heap_t* heap, size_t size) {
    stack_path += "nt_heap_malloc -> ";
    nt_assert(heap != nullptr);
    nt_assert(heap->thread_id == 0 || heap->thread_id == _nt_thread_id());

    info << "heap thread = " << heap->thread_id;

    void* p;
    if (nt_likely(size < NT_SMALL_SIZE_MAX)) {
        p = nt_heap_malloc_small(heap, size);
    } else {
        // p = _nt_malloc_generic(heap, size);
    }
    return {};
}

void* nt_malloc(size_t size) {
    stack_path += "nt_malloc -> ";
    return nt_heap_malloc(nt_get_default_heap(), size);
}

NT_NAMESPACE_END