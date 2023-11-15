#include "include/fd.h"
#include "include/defs.h"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unistd.h>

NT_NAMESPACE_BEGEN

file_discriptor::fd_wrapper::fd_wrapper(const value_type fd)
    : _fd(fd), _eof(false), _closed(false)
    , _read_count(0), _write_count(0) {
    if (_fd < 0) {
        throw std::runtime_error("invalid fd: " + std::to_string(_fd));
    }
}

file_discriptor::fd_wrapper::~fd_wrapper() {
    if (!_closed) close();
}

void file_discriptor::fd_wrapper::close() {
    ::close(_fd);
}

file_discriptor::file_discriptor(const value_type fd)
    : _internal_fd(std::make_shared<fd_wrapper>(fd)) {}

file_discriptor::file_discriptor(std::shared_ptr<fd_wrapper> dup)
    : _internal_fd(std::move(dup)) {}

ssize_t file_discriptor::read(std::string &buf, const value_type limit) {
    void* but_t;
    size_t buf_size = std::min<size_t>(1024 * 1024, limit);

    ssize_t read_len = ::read(get_fd(), but_t, buf_size);
    if (limit > 0 && read_len == 0) set_eof();
    if (read_len > static_cast<ssize_t>(buf_size))
        throw std::runtime_error("read() read more than requested");
    buf = static_cast<char*>(but_t);
    buf.resize(read_len);
    update_rd();

    return buf.size();
}

ssize_t file_discriptor::write() {
    // TODO
    return {};
}
ssize_t file_discriptor::send() {
    // TODO
    return {};
}
ssize_t file_discriptor::receive() {
    // TODO
    return {};
}
bool file_discriptor::is_readable() {
    // TODO
    return {};
}
bool file_discriptor::is_writable() {
    // TODO
    return {};
}
bool file_discriptor::set_timeout() {
    // TODO
    return {};
}
void file_discriptor::async_read() {
    // TODO
}
void file_discriptor::async_write() {
    // TODO
}
void file_discriptor::duplicate() const {
    // TODO
}
void file_discriptor::set_blocking() {
    // TODO
}
void file_discriptor::close() {
    // TODO
}

void    file_discriptor::update_rd() { _internal_fd->_read_count++; }
void    file_discriptor::update_wd() { _internal_fd->_write_count++; }
size_t  file_discriptor::get_fd()       const { return _internal_fd->_fd; }
bool    file_discriptor::eof()          const { return _internal_fd->_eof; }
bool    file_discriptor::is_closed()    const { return _internal_fd->_closed; }
ssize_t file_discriptor::get_rd_count() const { return _internal_fd->_read_count; }
ssize_t file_discriptor::get_wt_count() const { return _internal_fd->_write_count; }

NT_NAMESPACE_END