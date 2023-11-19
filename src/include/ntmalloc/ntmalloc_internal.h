#ifndef __LIBNT_MALLOC_INTERNAL_H
#define __LIBNT_MALLOC_INTERNAL_H

#include "ntmalloc.h"
#include "types.h"
#include <cstddef>
#include <cstdint>
#include <utility>

NT_NAMESPACE_BEGEN

/**
 * --------------------------------------
 * For Branch Prediction
 * --------------------------------------
 */
#if defined (__GNUC__) || defined (__clang__)
/**
 * @brief Hint that the expression x is `unlikely to be true in normal` circumstances
 * 
 * @param x The expression to be hinted
 * @return The hinted expression
 */
#define nt_unlikely(x)    __builtin_expect((x), 0)
/**
 * @brief Hint that the expression x is `likely to be true in normal` circumstances
 * 
 * @param x The expression to be hinted
 * @return The hinted expression
 */
#define nt_likely(x)      __builtin_expect((x), 1)
#else
#define nt_unlikely(x)
#define nt_likely(x)
#endif

/**
 * @brief Unused macro to suppress unused variable warnings.
 * 
 * @param x The variable to mark as unused.
 */
#define UNUSED(x)       (void)(x)
/**
 * @brief Macro to initialize 4, 8, 16, 32, 64, 128, or 256 instances of a given expression or function call.
 * 
 * @param x The expression or function call to be initialized.
 */
#define NT_INIT4(x)     x(), x(), x(), x()
#define NT_INIT8(x)     NT_INIT4(x), NT_INIT4(x)
#define NT_INIT16(x)    NT_INIT8(x), NT_INIT8(x)
#define NT_INIT32(x)    NT_INIT16(x), NT_INIT16(x)
#define NT_INIT64(x)    NT_INIT32(x), NT_INIT32(x)
#define NT_INIT128(x)   NT_INIT64(x), NT_INIT64(x)
#define NT_INIT256(x)   NT_INIT128(x), NT_INIT128(x)

/**
 * @brief Macro to define empty page queues for every "bin"(class).
 * 
 * !This macro initializes empty page queues for different size classes, with each queue being initialized as empty (nullptr, nullptr, size * sizeof(uintptr_t)).
 * !The size of each queue is determined by the parameter passed to the QEMPTY macro.
 *
 * @note "bin" can be translated as "stack statistics", so this should be understood as initializing each heap of a different type size
 */
#define QEMPTY(sz)  { nullptr, nullptr, (sz) * sizeof(uintptr_t) }
#define NT_PAGE_QUEUES_EMPTY \
  { QEMPTY(1), \
    QEMPTY(1),     QEMPTY(2),     QEMPTY(3),     QEMPTY(4),     QEMPTY(5),     QEMPTY(6),     QEMPTY(7),     QEMPTY(8), \
    QEMPTY(10),    QEMPTY(12),    QEMPTY(14),    QEMPTY(16),    QEMPTY(20),    QEMPTY(24),    QEMPTY(28),    QEMPTY(32), \
    QEMPTY(40),    QEMPTY(48),    QEMPTY(56),    QEMPTY(64),    QEMPTY(80),    QEMPTY(96),    QEMPTY(112),   QEMPTY(128), \
    QEMPTY(160),   QEMPTY(192),   QEMPTY(224),   QEMPTY(256),   QEMPTY(320),   QEMPTY(384),   QEMPTY(448),   QEMPTY(512), \
    QEMPTY(640),   QEMPTY(768),   QEMPTY(896),   QEMPTY(1024),  QEMPTY(1280),  QEMPTY(1536),  QEMPTY(1792),  QEMPTY(2048), \
    QEMPTY(2560),  QEMPTY(3072),  QEMPTY(3584),  QEMPTY(4096),  QEMPTY(5120),  QEMPTY(6144),  QEMPTY(7168),  QEMPTY(8192), \
    QEMPTY(10240), QEMPTY(12288), QEMPTY(14336), QEMPTY(16384), QEMPTY(20480), QEMPTY(24576), QEMPTY(28672), QEMPTY(32768), \
    QEMPTY(40960), QEMPTY(49152), QEMPTY(57344), QEMPTY(65536), QEMPTY(81920), QEMPTY(98304), QEMPTY(114688), \
    QEMPTY(NT_LARGE_WSIZE_MAX + 1  /*131072, Huge queue */), \
    QEMPTY(NT_LARGE_WSIZE_MAX + 2) /* Full queue */ }

/**
 * @brief Calculates the word size from the given size
 * 
 * This function calculates the word size from the given size.
 * 
 * @param size The size for which to calculate the word size
 * @return The calculated word size
 */
static inline size_t _nt_wsize_from_size(size_t size) {
  return (size + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);
}

/**
 * @brief default heap to allocate from
 */
extern nt_thread(nt_heap_t*) _nt_heap_default;
/**
 * @brief An empty heap
 */
extern const nt_heap_t _nt_heap_empty;

/**
 * @brief Retrieves the default heap
 * 
 * This function retrieves the default heap.
 * 
 * @return A pointer to the default heap
 */
static inline nt_heap_t* nt_get_default_heap(void) {
  return _nt_heap_default;
}

/**
 * @brief Retrieves a free small page from the heap
 * 
 * This function retrieves a free small page of the given size from the specified heap.
 * 
 * @param heap The heap from which to retrieve the free page
 * @param size The size of the page to retrieve
 * @return A pointer to the retrieved free page
 */
static inline nt_page_t* _nt_heap_get_free_small_page(nt_heap_t* heap, size_t size) {
  nt_assert_internal(size <= NT_SMALL_SIZE_MAX);
  return heap->pages_free_direct[_nt_wsize_from_size(size)];
}

#if defined (__GNUC__) || defined (__clang__)
/**
 * @brief TLS register on x86 is in the FS or GS register
 * 
 * @ref see: https://akkadia.org/drepper/tls.pdf
 */
static inline uintptr_t _nt_thread_id(void) {
  uintptr_t tid;
  #if defined (__i386__)
  __asm__("movl %%gs:0, %0" : "=r" (tid) : :);  // IA32 32 bit always uses GS
  #elif defined (__x86_64__)
  __asm__("movq %%fs:0, %0" : "=r" (tid) : :); // x86_64 Linux uses FS
  #elif defined (__arm__)
  asm volatile ("mrc p15, 0, %0, c13, c0, 3" : "=r" (tid));
  #endif

  return tid;
}
#endif

NT_NAMESPACE_END

#endif //! __LIBNT_MALLOC_INTERNAL_H