#include "../ntmalloc.h"
#include "../ntmalloc_internal.h"

/**
 * @brief Performs an `empty initialization` by default, 
 * with special initialization for `flags` and `thread_free`
 */
nt_page_t _nt_page_empty = {
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

const nt_heap_t _nt_heap_empty = {
    nullptr, NT_SMALL_PAGE_EMPTY, 
    {}, nullptr, 0, 0, 0, 0, false
};