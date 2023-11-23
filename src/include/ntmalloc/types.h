#ifndef __LIBNT_MALLOC_DEFS_H
#define __LIBNT_MALLOC_DEFS_H

#include "../defs.h"

#include <cstddef>
#include <cstdint>

NT_NAMESPACE_BEGEN

struct nt_tld;
struct nt_heap;

typedef nt_heap nt_heap_t;
typedef nt_tld  nt_tld_t;

/**
 * @brief This comment discusses the size and relationship between pointers and intptr_t in C or C++. 
 * It notes the assumption that `sizeof(void*)` is equal to `sizeof(intptr_t)` on the platforms known to the author. 
 * Additionally, it points out that while the C standard only requires that a pointer can be cast to intptr_t and back, 
 * the author also needs the guarantee that an intptr_t can be cast to a pointer. This is to ensure portability and compatibility across different platforms.
 * 
 * @code {.cc}
 * p == (void*)((intptr_t)p)) //! the C standard only requires this form
 * i == (intptr_t)((void*)i)  //! but we need The two types can be safely converted to each other
 * @endcode
 */
#if INTPTR_MAX == 9223372036854775807LL
    constexpr const size_t NT_INTPTR_SHIFT = 3;
#elif INTPTR_MAX == 2147483647LL
    constexpr const size_t NT_INTPTR_SHIFT = 2;
#else
#   error platform must be 32 or 64 bits
#endif

/**
 * @brief `NT_INTPTR_SIZE` is 8 byte, 
 */
constexpr const size_t NT_INTPTR_SIZE       = 1 << NT_INTPTR_SHIFT;

/**
 * @brief `NT_SMALL_PAGE_SHIFT` is 16, 1 << 16 = 0x10000 = 64kb
 */
constexpr const size_t NT_SMALL_PAGE_SHIFT  = 13 + NT_INTPTR_SHIFT;
/**
 * @brief `NT_LARGE_PAGE_SHIFT` is 22, 1 << 22 = 0x400000 = 4mb
 */
constexpr const size_t NT_LARGE_PAGE_SHIFT  = 6 + NT_SMALL_PAGE_SHIFT;
/**
 * @brief `NT_SEGMENT_SHIFT` = `NT_LARGE_PAGE_SHIFT`
 */
constexpr const size_t NT_SEGMENT_SHIFT     = NT_LARGE_PAGE_SHIFT;

/**
 * @brief `NT_SEGMENT_SIZE` is 4mb
 */
constexpr const size_t NT_SEGMENT_SIZE      = 1 << NT_SEGMENT_SHIFT;
constexpr const size_t NT_SEGMENT_MASK      = (uintptr_t)NT_SEGMENT_SIZE - 1;

/**
 * @brief small page is set 64kb
 */
constexpr const size_t NT_SMALL_PAGE_SIZE   = 1 << NT_SMALL_PAGE_SHIFT;
/**
 * @brief large page is set 4mb
 */
constexpr const size_t NT_LARGE_PAGE_SIZE   = 1 << NT_LARGE_PAGE_SHIFT;

constexpr const size_t NT_SMALL_PAGES_PER_SEGMENT = NT_SEGMENT_SIZE / NT_SMALL_PAGE_SIZE;
constexpr const size_t NT_LARGE_PAGES_PER_SEGMENT = NT_SEGMENT_SIZE / NT_LARGE_PAGE_SIZE;

constexpr const size_t NT_SMALL_WSIZE_MAX   = 128;
constexpr const size_t NT_SMALL_SIZE_MAX    = NT_SMALL_WSIZE_MAX * sizeof(void*);

constexpr const size_t NT_LARGE_SIZE_MAX    = NT_LARGE_PAGE_SIZE / 8;
constexpr const size_t NT_LARGE_WSIZE_MAX   = NT_LARGE_SIZE_MAX >> NT_INTPTR_SHIFT;

/**
 * @brief A block containing a free list
 */
typedef struct nt_block {
    using nt_encode_t = uintptr_t;
    nt_encode_t next;           // Pointer to the next free block
} nt_block_t;

typedef enum nt_delayed {
    NT_NO_DELAYED_FREE = 0,
    NT_USE_DELAYED_FREE,
    NT_DELAYED_FREEING,
} nt_delayed_t;

typedef union nt_page_flags {
    uint16_t value;
    struct {
        bool has_aligned;
        bool is_full;
    };
} nt_page_flags_t;

/**
 * @brief Thread free list.  
 *
 * We use 2 bits of the pointer for the `use_delayed_free` and `delayed_freeing` flags.
 */
typedef struct nt_thread_free {
    uintptr_t value;
    struct {
        nt_delayed_t delayed: 2;
        uintptr_t    head: 62;      // head free block in the list (right-shifted by 2)
    };
} nt_thread_free_t;

/**
 * @brief A page contains blocks of one specific size (`block_size`).
 * Each page has three list of free blocks:
 * `free` for blocks that can be allocated,
 * `local_free` for freed blocks that are not yet available to `mi_malloc`
 * `thread_free` for freed blocks by other threads
 * The `local_free` and `thread_free` lists are migrated to the `free` list
 * when it is exhausted. The separate `local_free` list is necessary to
 * implement a monotonic heartbeat. The `thread_free` list is needed for
 * avoiding atomic operations in the common case.
 *
 * `used - thread_freed` == actual blocks that are in use (alive)
 * `used - thread_freed + |free| + |local_free| == capacity`
 *
 * @note: we don't count `freed` (as |free|) instead of `used` to reduce
 *       the number of memory accesses in the `mi_page_all_free` function(s).
 * @note: the funny layout here is due to:
 * - access is optimized for `mi_free` and `mi_page_alloc`
 * - using `uint16_t` does not seem to slow things down
 * 
 */
typedef struct nt_page {
    uint8_t             segment_idx;
    bool                segment_used;
    bool                is_reset;

    nt_page_flags_t     flags;
    uint16_t            capacity;
    uint16_t            reserved;
    
    nt_block_t*         free;               //! list of available free blocks (`malloc` allocates from this list)
    uintptr_t           cookie;
    size_t              used;

    nt_block_t*         local_free;
    volatile uintptr_t  thread_freed;
    volatile nt_thread_free_t thread_free;

    size_t              block_size;
    nt_heap_t*          heap;
    nt_page*            next;
    nt_page*            prev;
} nt_page_t;

typedef enum nt_page_kind {
    NT_PAGE_SAMLL,
    NT_PAGE_LARGE,
    NT_PAGE_HUGE,
} nt_page_kind_t;

/**
 * @brief Segments are large allocated memory blocks (2mb on 64 bit) from
 * the OS. Inside segments we allocated fixed size _pages_ that contain blocks.
 */
typedef struct nt_segment {
    nt_segment*     next;
    nt_segment*     prev;
    nt_segment*     abandoned_next;
    size_t          abandoned;
    size_t          used;
    size_t          capacity;       // count of available pages (`#free + used`)
    size_t          segment_size;
    size_t          segment_info_size;
    uintptr_t       cookie;

    size_t          page_shift;
    uintptr_t       thread_id;
    nt_page_kind_t  page_kind;      // kind of pages: small, large, or huge
    nt_page_t       pages[1];
} nt_segment_t;

typedef struct nt_page_queue {
    nt_page_t* first;
    nt_page_t* last;
    size_t     block_size;
} nt_page_queue_t;

/**
 * @brief Constant representing the size of a huge bin.
 */
constexpr const int NT_BIN_HUGE        = 64U;
/**
 * @brief Constant representing the size of a huge bin.
 */
constexpr const int NT_BIN_FULL        = NT_BIN_HUGE + 1;

typedef struct nt_heap {
    nt_tld_t*               tld;
    nt_page_t*              pages_free_direct[NT_SMALL_WSIZE_MAX + 2];  //! optimize: array where every entry points a page with possibly free blocks in the corresponding queue for that size.
    nt_page_queue_t         pages[NT_BIN_FULL + 1];                     //! queue of pages for each size class (or "bin")
    volatile nt_block_t*    thread_delayed_free;
    uintptr_t               thread_id;
    uintptr_t               cookie;
    uintptr_t               random;
    size_t                  page_count;
    bool                    no_reclaim;
} nt_heap_t;

/**
 * --------------------------------------------
 * DEBUG
 * --------------------------------------------
 */

#if !defined (NT_DEBUG)
#if !defined (NDEBUG) || defined (_DEBUG)
#   define NT_DEBUG 1
#else
#   define NT_DEBUG 0
#endif
#endif 

/**
 * --------------------------------------------
 * Statistics
 * --------------------------------------------
 */

#ifndef NT_STAT
#   if NT_DEBUG > 0
#       define NT_STAT 2
#   else
#       define NT_STAT 0
#   endif
#endif

typedef struct nt_stat_count {
    int64_t allocated;
    int64_t freed;
    int64_t peak;
    int64_t current;
} nt_stat_count_t;

typedef struct nt_stat_counter {
    int64_t total;
    int64_t count;
} nt_stat_counter_t;

typedef struct nt_stats {
    nt_stat_count_t segments;
    nt_stat_count_t pages;
    nt_stat_count_t reserved;
    nt_stat_count_t committed;
    nt_stat_count_t reset;
    nt_stat_count_t segments_abandoned;
    nt_stat_count_t pages_abandoned;
    nt_stat_count_t pages_extended;
    nt_stat_count_t mmap_calls;
    nt_stat_count_t mmap_right_align;
    nt_stat_count_t mmap_ensure_aligned;
    nt_stat_count_t threads;
    nt_stat_count_t huge;
    nt_stat_count_t malloc;
    nt_stat_counter_t seraches;
#ifdef NT_STAT
#   if NT_STAT > 1
    nt_stat_count_t normal[NT_BIN_HUGE + 1];
#   endif
#endif
} nt_stats_t;

typedef struct nt_segment_queue {
    nt_segment_t* first;
    nt_segment_t* last;
} nt_segment_queue_t;

typedef struct nt_segments_tld {
    nt_segment_queue_t small_free;
    size_t             current_size;
    size_t             peak_size;
    size_t             cache_count;
    size_t             cache_size;
    nt_segment_queue_t cache;
    nt_stats_t*        stats;
} nt_segments_tld_t;

typedef struct nt_os_tld {
    uintptr_t           mmap_next_probable;
    void*               mmap_previous;
    uint8_t*            pool;
    size_t              pool_available;
    nt_stats_t*         stats;
} nt_os_tld_t;

typedef struct nt_tld {
    unsigned long long  heartbeat;
    nt_heap_t*          heap_backing;
    nt_segments_tld_t   segments;
    nt_os_tld_t         os;
    nt_stats_t          stats;
} nt_tld_t;


#if (NT_DEBUG)
//! use our own assertion to print without memory allocation
// TODO void _nt_assert_fail(const char* assertion, const char* fname, unsigned int line, const char* func)
#define nt_assert(expr)     ((expr) ? (void)0 : exit(1))
#else
#define nt_assert(x)
#endif 

#if (NT_DEBUG > 1)
#define nt_assert_internal    nt_assert
#else
#define nt_assert_internal(x)
#endif

#if (NT_DEBUG > 2)
#define nt_assert_expensive   nt_assert
#else
#define nt_assert_expensive(x)
#endif

void _nt_stat_decrease(nt_stat_count_t* stat, size_t amount);

#if (NT_STAT)
#   define nt_stat_decrease(stat, amount)       _nt_stat_decrease(&(stat), amount)
#endif

NT_NAMESPACE_END

#endif //! __LIBNT_MALLOC_DEFS_H