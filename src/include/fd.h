#ifndef __LIBNT_FILE_DISCRIPTOR_H
#define __LIBNT_FILE_DISCRIPTOR_H

#include "defs.h"

#include <cstddef>
#include <string>
#include <cmath>
#include <unistd.h>

#include <algorithm>
#include <utility>
#include <limits>
#include <memory>
#include <stdexcept>
#include <sys/types.h>

NT_NAMESPACE_BEGEN

class file_discriptor {
    using __self            = file_discriptor*;
    using __self_const      = const file_discriptor*;
    using __self_ref        = file_discriptor&;
    using __self_ref_const  = const file_discriptor&;
    using value_type    = size_t;
    using flag_type     = bool;
    using ret_type      = ssize_t;
    using limits        = std::numeric_limits<value_type>;

    struct fd_wrapper {
        value_type _fd;
        flag_type  _eof;
        flag_type  _closed;
        ret_type   _read_count;
        ret_type   _write_count;

        explicit fd_wrapper(value_type fd);
        ~fd_wrapper();

        void close();

        fd_wrapper(const fd_wrapper&)   = delete;
        fd_wrapper(fd_wrapper&&)        = delete;
        fd_wrapper& operator= (const fd_wrapper&)   = delete;
        fd_wrapper& operator= (fd_wrapper&&)        = delete;
    };

    std::shared_ptr<fd_wrapper> _internal_fd;

private:
    explicit file_discriptor(std::shared_ptr<fd_wrapper> dup);

    void update_rd();
    void update_wd();
public:
    explicit file_discriptor(value_type fd);
    ~file_discriptor() = default;

    file_discriptor(__self_ref_const)   = delete;
    file_discriptor(__self_ref&)        = delete;
    __self_ref operator= (__self_ref_const) = delete;
    __self_ref operator= (__self_ref&)      = delete;

    // TODO
    ret_type read(std::string& buf, const value_type limit = limits::max());
    ret_type write();
    ret_type send();
    ret_type receive();
    flag_type is_readable();
    flag_type is_writable();
    flag_type set_timeout();
    void async_read();
    void async_write();
    void duplicate() const;
    void set_blocking();
    void close();
    // TODO future
    // flag_type enable_tls();
    // flag_type enable_multicast();
    // flag_type enable_broadcast();
    // ret_type  compress_send();
    // ret_type  receive_decompress();
    // ret_type  zero_copy();
    // ret_type  batch_send();
public:
    void       set_eof();
    value_type get_fd() const;
    flag_type  eof() const;
    flag_type  is_closed() const;
    ret_type   get_rd_count() const;
    ret_type   get_wt_count() const;
};

NT_NAMESPACE_END

#endif //! __LIBNT_FILE_DISCRIPTOR_H