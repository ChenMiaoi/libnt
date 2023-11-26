#ifndef __LIBNT_MALLOC_ATOMIC_H
#define __LIBNT_MALLOC_ATOMIC_H

#include "../defs.h"

#include <atomic>
#include <cstdint>
#include <thread>

NT_NAMESPACE_BEGEN

/**
 * @brief Converts the given value to the corresponding std::atomic type.
 * 
 * @tparam _Atc The atomic type
 * @param value The given value
 * @return std::atomic<_Atc>& The converted std::atomic type reference
 */
template <typename _Atc>
std::atomic<_Atc>& to_atomic(volatile _Atc& value) {
    return *reinterpret_cast<std::atomic<_Atc>*>(const_cast<_Atc*>(&value));
}

/**
 * @brief Performs atomic increment operation on a value of type uintptr_t.
 * 
 * @param p Pointer to uintptr_t
 * @return uintptr_t The value after increment
 */
static inline uintptr_t nt_atomic_increment(volatile uintptr_t* p) {
    return to_atomic(*p).fetch_add(static_cast<uintptr_t>(1), std::memory_order_relaxed) + 1;
}

/**
 * @brief Performs atomic increment operation on a value of type uint32_t.
 * 
 * @param p Pointer to uint32_t
 * @return uint32_t The value after increment
 */
static inline uint32_t nt_atomic_increment32(volatile uint32_t *p) {
    return to_atomic(*p).fetch_add(static_cast<uint32_t>(1), std::memory_order_relaxed) + 1;
}

/**
 * @brief Performs atomic decrement operation on a value of type uintptr_t.
 * 
 * @param p Pointer to uintptr_t
 * @return uintptr_t The value after decrement
 */
static inline uintptr_t nt_atomic_decrement(volatile uintptr_t* p) {
    return to_atomic(*p).fetch_sub(static_cast<uintptr_t>(1), std::memory_order_relaxed) - 1;
}

/**
 * @brief Performs atomic addition operation on a value of type int64_t.
 * 
 * @param p Pointer to int64_t
 * @param add Value to add
 * @return int64_t The value after addition
 */
static inline int64_t nt_atomic_add(volatile int64_t* p, int64_t add) {
    return to_atomic(*p).fetch_add(add, std::memory_order_relaxed) + add;
}

/**
 * @brief Performs atomic subtraction operation on a value of type uintptr_t.
 * 
 * @param p Pointer to uintptr_t
 * @param sub Value to subtract
 * @return uintptr_t The value after subtraction
 */
static inline uintptr_t nt_atomic_subtract(volatile uintptr_t* p, uintptr_t sub) {
    return to_atomic(*p).fetch_sub(sub, std::memory_order_relaxed) - sub;
}

/**
 * @brief Performs atomic subtraction operation on a value of type uint32_t.
 * 
 * @param p Pointer to uint32_t
 * @param sub Value to subtract
 * @return uint32_t The value after subtraction
 */
static inline uintptr_t nt_atomic_subtract32(volatile uint32_t* p, uint32_t sub) {
    return to_atomic(*p).fetch_sub(sub, std::memory_order_relaxed) - sub;
}

/**
 * @brief Performs atomic compare-and-exchange operation on a value of type uint32_t.
 * 
 * @param p Pointer to uint32_t
 * @param exchange Value to exchange
 * @param compare Value to compare
 * @return bool True if the comparison is successful and the exchange is performed, false otherwise
 */
static inline bool nt_atomic_compare_exchange32(volatile uint32_t* p, uint32_t exchange, uint32_t compare) {
    return to_atomic(*p).compare_exchange_weak(compare, exchange, std::memory_order_relaxed, std::memory_order_seq_cst);
}

/**
 * @brief Performs atomic compare-and-exchange operation on a value of type uintptr_t.
 * 
 * @param p Pointer to uintptr_t
 * @param exchange Value to exchange
 * @param compare Value to compare
 * @return bool True if the comparison is successful and the exchange is performed, false otherwise
 */
static inline bool nt_atomic_compare_exchange(volatile uintptr_t* p, uintptr_t exchange, uintptr_t compare) {
    return to_atomic(*p).compare_exchange_weak(compare, exchange, std::memory_order_relaxed, std::memory_order_seq_cst);
}

/**
 * @brief Performs atomic exchange operation on a value of type uintptr_t.
 * 
 * @param p Pointer to uintptr_t
 * @param exchange Value to exchange
 * @return uintptr_t The value before the exchange
 */
static inline uintptr_t nt_atomic_exchange(volatile uintptr_t* p, uintptr_t exchange) {
    return to_atomic(*p).exchange(exchange, std::memory_order_relaxed);
}

#if defined (__cplusplus)
static inline void nt_atomic_yield(void) {
    std::this_thread::yield();
}
#elif defined(__GNUC__) || defined(__clang__)
#   if defined(__x86_64__) || defined(__i386__)
static inline void nt_atomic_yield(void) {
    asm volatile ("pause" ::: "memory");
}
#   elif defined(__arm__) || defined(__aarch64__)
static inline void nt_atomic_yield(void) {
    asm volatile("yield");
}
#   endif
#else
#include <unistd.h>
static inline void nt_atomic_yield(void) {
    sleep(0);
}
#endif

NT_NAMESPACE_END

#endif //! __LIBNT_MALLOC_ATOMIC_H