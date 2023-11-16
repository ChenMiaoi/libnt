#ifndef __LIBNT_FILE_DISCRIPTOR_H
#define __LIBNT_FILE_DISCRIPTOR_H

#include "defs.h"

#include <cstddef>
#include <string>
#include <cmath>
#include <unistd.h>
#include <fcntl.h>

#include <algorithm>
#include <utility>
#include <limits>
#include <memory>
#include <stdexcept>
#include <sys/types.h>

NT_NAMESPACE_BEGEN

/**
 * @brief The `file_discriptor` is a wrapper for the native file descriptor
 * used as a prerequisite for implementing various encapsulations.
 */
class file_discriptor {
    using __self            = file_discriptor*;
    using __self_const      = const file_discriptor*;
    using __self_ref        = file_discriptor&;
    using __self_ref_const  = const file_discriptor&;
    using value_type    = size_t;
    using flag_type     = bool;
    using ret_type      = ssize_t;
    using limits        = std::numeric_limits<value_type>;

    /**
     * @brief The `fd_wrapper` struct is a wrapper for a file descriptor.
     */
    struct fd_wrapper {
        value_type _fd;         // The raw file descriptor.
        flag_type  _eof;        // Flag indicating if the end of file has been reached.
        flag_type  _closed;     // Flag indicating if the file descriptor is closed.
        ret_type   _read_count; // The count of reader.
        ret_type   _write_count;// The count of writer.

        explicit fd_wrapper(const value_type fd);
        ~fd_wrapper();

        /**
         * @brief Close the file descriptor.
         * 
         * This function closes the file descriptor, releasing any system resources associated with it.
         */
        void close();
        /**
         * @brief Check if the file descriptor is valid.
         * 
         * This function checks if the file descriptor is valid and can be used for I/O operations.
         * 
         * @return true if the file descriptor is valid, false otherwise.
         */
        flag_type  is_valid() const;

        fd_wrapper(const fd_wrapper&)   = delete;
        fd_wrapper(fd_wrapper&&)        = delete;
        fd_wrapper& operator= (const fd_wrapper&)   = delete;
        fd_wrapper& operator= (fd_wrapper&&)        = delete;
    };

    std::shared_ptr<fd_wrapper> _internal_fd;

private:
    explicit file_discriptor(std::shared_ptr<fd_wrapper> dup);

    void update_rd();
    void update_wt();
public:
    /**
     * @brief Construct a file_discriptor object with the given file descriptor.
     * @param fd The file descriptor to be wrapped.
     */
    explicit file_discriptor(value_type fd);
    ~file_discriptor() = default;

    file_discriptor(__self_ref_const)           = delete;
    file_discriptor(file_discriptor&&)          = default;
    __self_ref operator= (__self_ref_const)     = delete;
    __self_ref operator= (file_discriptor&&)    = default;

    /**
     * @brief Read data from the file descriptor.
     * 
     * @param limit The maximum number of bytes to read.
     * @return The data read from the file descriptor.
     */
    std::string read(const value_type limit = limits::max());
    /**
     * @brief Read data from the file descriptor into the given buffer.
     * 
     * @param buf The buffer to read the data into.
     * @param limit The maximum number of bytes to read.
     * @return The number of bytes read.
     */
    ret_type read(std::string& buf, const value_type limit = limits::max());
    /**
     * @brief Write data to the file descriptor.
     * 
     * @param src The data to be written.
     * @param buf_len The length of the data.
     * @return The number of bytes written.
     */
    ret_type write(const char* src, const value_type buf_len);
    /**
     * @brief Write data to the file descriptor.
     * 
     * @param src The data to be written.
     * @param limit The maximum number of bytes to write.
     * @return The number of bytes written.
     */
    ret_type write(const std::string& src, const value_type limit);
    /**
     * @brief Send data through the file descriptor.
     * 
     * @param src The data to be sent.
     * @param buf_len The length of the data.
     * @return The number of bytes sent.
     */
    ret_type send(const char* src, const value_type buf_len);
    /**
     * @brief Receive data from the file descriptor.
     * 
     * @param buf The buffer to receive the data into.
     * @param limit The maximum number of bytes to receive.
     * @return The number of bytes received.
     */
    ret_type receive(std::string& buf, const value_type limit = limits::max());
    /**
     * @brief Check if the file descriptor is readable.
     * 
     * @return true if the file descriptor is readable, false otherwise.
     */
    flag_type is_readable();
    /**
     * @brief Check if the file descriptor is writable.
     * 
     * @return true if the file descriptor is writable, false otherwise.
     */
    flag_type is_writable();
    /**
     * @brief Create a duplicate of the file descriptor.
     * 
     * @return A new file_discriptor object that shares the same file descriptor.
     */
    file_discriptor duplicate() const;
    /**
     * @brief Set the file descriptor to blocking mode.
     */
    void set_blocking();
    /**
     * @brief Close the file descriptor.
     */
    void close();
    // TODO future
    // flag_type set_timeout(const value_type seconds, const value_type microseconds);
    // void async_read();
    // void async_write();
    // flag_type enable_tls();
    // flag_type enable_multicast();
    // flag_type enable_broadcast();
    // ret_type  compress_send();
    // ret_type  receive_decompress();
    // ret_type  zero_copy();
    // ret_type  batch_send();
public:
    /**
     * @brief Set the end of file flag.
     */
    void       set_eof();
    /**
     * @brief Get the file descriptor.
     * 
     * @return The file descriptor.
     */
    value_type get_fd() const;
    /**
     * @brief Check if the end of file has been reached.
     * 
     * @return true if the end of file has been reached, false otherwise.
     */
    flag_type  eof() const;
    /**
     * @brief Check if the file descriptor is in blocking mode.
     * 
     * @return true if the file descriptor is in blocking mode, false otherwise.
     */
    flag_type  is_blocking() const;
    /**
     * @brief Check if the file descriptor is closed.
     * 
     * @return true if the file descriptor is closed, false otherwise.
     */
    flag_type  is_closed() const;
    /**
     * @brief Get the count of readers.
     * 
     * @return The count of readers.
     */
    ret_type   get_rd_count() const;
    /**
     * @brief Get the count of writers.
     * 
     * @return The count of writers.
     */
    ret_type   get_wt_count() const;
};

NT_NAMESPACE_END

#endif //! __LIBNT_FILE_DISCRIPTOR_H