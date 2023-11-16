#ifndef __LIBNT_DEFS_H
#define __LIBNT_DEFS_H

#ifndef NT_NAMESPACE_BEGIN
#   define NT_NAMESPACE_BEGEN namespace nt {
#endif

#ifndef NT_NAMESPACE_END
#define NT_NAMESPACE_END }
#endif

#ifndef NT_LOGGER
#define NT_LOGGER
#include "log.h"
#endif 

#endif //! __LIBNT_DEFS_H