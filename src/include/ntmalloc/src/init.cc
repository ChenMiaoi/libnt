#include "../ntmalloc.h"
#include "../ntmalloc_internal.h"

NT_NAMESPACE_BEGEN

/**
 * @brief Performs an `empty initialization` by default, 
 * with special initialization for `flags` and `thread_free`
 */
const nt_page_t _nt_page_empty = {
    .flags = {0},
    .thread_free = {0},
};

#define NT_PAGE_EMPTY() ((nt_page_t*)&_nt_page_empty)

/**
 * @brief Macro to define a small page with empty initialization.
 * 
 * This macro initializes a small page with `130 instances` of a given expression or function call, 
 * followed by two additional instances of NT_PAGE_EMPTY.
 */
#define NT_SMALL_PAGE_EMPTY \
    { NT_INIT128(NT_PAGE_EMPTY), NT_PAGE_EMPTY(), NT_PAGE_EMPTY() }

/**
 * @brief Statically allocate an empty heap as the initial thread local value for the default heap, 
 * and statically allocate the backing heap for the main thread so it can function without doing any allocation itself 
 * (as accessing a thread local for the first time may lead to allocation itself on some platforms)
 */
const nt_heap_t _nt_heap_empty = {
    .pages_free_direct = NT_SMALL_PAGE_EMPTY,
    .pages = NT_PAGE_QUEUES_EMPTY,
};

nt_thread(nt_heap_t*) _nt_heap_default = (nt_heap_t*)&_nt_heap_empty;

NT_NAMESPACE_END