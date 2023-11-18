#ifndef __LIBNT_MALLOC_INTERNAL_H
#define __LIBNT_MALLOC_INTERNAL_H

#include "types.h"
#include <utility>

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
    QEMPTY(MI_LARGE_WSIZE_MAX + 1  /*131072, Huge queue */), \
    QEMPTY(MI_LARGE_WSIZE_MAX + 2) /* Full queue */ }

#endif //! __LIBNT_MALLOC_INTERNAL_H