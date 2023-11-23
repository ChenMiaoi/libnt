#include "../ntmalloc.h"
#include "../ntmalloc_internal.h"
#include <cstddef>
#include <string>

extern std::string stack_path;

NT_NAMESPACE_BEGEN


void* _nt_malloc_generic(nt_heap_t* heap, size_t size) {
    stack_path += "_nt_malloc_generic -> ";
    nt_assert_internal(heap != nullptr);

    //! initialize if necessary
    if (nt_unlikely(!nt_heap_is_initialized(heap))) {
        nt_thread_init();
        // TODO
    }
    // TODO
    return {};
}

NT_NAMESPACE_END