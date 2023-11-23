#include "../ntmalloc.h"
#include "../ntmalloc_internal.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <pthread.h>
#include <string>

extern std::string stack_path;

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

/**
 * @brief Macro for accessing the statistics of the main thread-local data (tld_main).
 * 
 * This macro is used to obtain a pointer to the statistics within the main thread-local data (tld_main).
 */
#define tld_main_stats ((nt_stats_t*)((uint8_t*)&tld_main + offsetof(nt_tld_t, stats)))

static nt_tld_t tld_main = {
      .heap_backing = &_nt_heap_main,
      .segments = { .stats = tld_main_stats },
      .os = { .stats = tld_main_stats },
      .stats = { NT_STAT_EMPTY },
};

nt_heap_t _nt_heap_main = {
    .tld = &tld_main,
    .pages_free_direct = NT_SMALL_PAGE_EMPTY,
    .pages = NT_PAGE_QUEUES_EMPTY,
};

/**
 * -----------------------------------------
 * Initialization of random numbers
 * -----------------------------------------
 */

constexpr uintptr_t _nt_random_shuffle(uintptr_t x) {
    // by Sebastiano Vigna, see: <http://xoshiro.di.unimi.it/splitmix64.c>
    if constexpr (NT_INTPTR_SIZE == 8) {
        x ^= x >> 30;
        x *= 0xbf58476d1ce4e5b9UL;
        x ^= x >> 27;
        x *= 0x94d049bb133111ebUL;
        x ^= x >> 31;
    } else if constexpr (NT_INTPTR_SIZE == 4) {
    // by Chris Wellons, see: <https://nullprogram.com/blog/2018/07/31/>
        x ^= x >> 16;
        x *= 0x7feb352dUL;
        x ^= x >> 15;
        x *= 0x846ca68bUL;
        x ^= x >> 16;
    }
    return x;
}

uintptr_t _nt_random_init(uintptr_t seed) {
    stack_path += "_nt_random_init -> ";
    return {};
}

/**
 * ---------------------------------------------------------
 * Run functions on process init/done, and thread init/done
 * ---------------------------------------------------------
 */

bool _nt_process_is_initialized = false;    // set to `true` in `nt_process_init`.
static pthread_key_t nt_pthread_key;

static void nt_pthread_done(void* value) {
    if (value != nullptr) nt_thread_done();
}

void nt_thread_done(void) {
    nt_heap_t* heap = nt_get_default_heap();
    if (!_nt_is_main_thread() && nt_heap_is_initialized(heap)) {
        nt_stat_decrease(heap->tld->stats.threads, 1);
    }
}

/**
 * @brief Set up handlers so `nt_thread_done` is called automatically
 */
static void nt_process_setup_auto_thread_done(void) {
    stack_path += "nt_process_setup_auto_thread_done -> ";
    static bool tls_initialized = false;
    if (tls_initialized) return;
    tls_initialized = true;
    pthread_key_create(&nt_pthread_key, nt_pthread_done);
    info << "pthread_key = " << nt_pthread_key;
}

nt_thread(nt_heap_t*) _nt_heap_default = (nt_heap_t*)&_nt_heap_empty;

bool _nt_is_main_thread(void) {
    return (_nt_heap_main.thread_id == 0 || _nt_heap_main.thread_id == _nt_thread_id());
}

void nt_thread_init(void) {
    stack_path += "nt_thread_init -> ";
    nt_process_init();
    // TODO
}

static void nt_process_done(void);

auto dec2hex = [](auto x) {
    char buffer[32];
    auto ret = std::to_chars(buffer, buffer + 32, x, 16);
    *ret.ptr = '\0';
    return std::string(buffer);
};

void nt_process_init(void) {
    stack_path += "nt_process_init -> ";
    //! ensure we are called once
    if (_nt_process_is_initialized) return;
    _nt_process_is_initialized = !_nt_process_is_initialized;

    _nt_heap_main.thread_id = _nt_thread_id();
    // _nt_verbose_message("process init: 0x%zx\n", _nt_heap_main.thread_id);
    info << "process init: 0x" << dec2hex(_nt_heap_main.thread_id);

    uintptr_t random = _nt_random_init(_nt_heap_main.thread_id);
    info << "random = " << random;

    _nt_heap_main.cookie = (uintptr_t)&_nt_heap_main ^ random;
    _nt_heap_main.random = _nt_random_shuffle(random);
    info << "cookie = " << _nt_heap_main.cookie;
    info << "random = " << _nt_heap_main.random;
#if NT_DEBUG
    // _nt_verbose_message("debug level : %d\n", MI_DEBUG);
    info << "debug level: " << NT_DEBUG;
#endif
    atexit(&nt_process_done);
    // TODO
    nt_process_setup_auto_thread_done();
    // nt_stats_reset();
}   

/**
 * @brief 
 * TODO
 */
static void nt_process_done(void) {
    stack_path += "nt_process_done -> ";
    //! only shutdown if we were initialized
    if (!_nt_process_is_initialized) return;

    //! ensure we are called once
    static bool process_done = false;
    if (process_done) return;
    process_done = !process_done;

#ifndef NDEBUG
    // TODO
#endif
    // TODO
}

NT_NAMESPACE_END