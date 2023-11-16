#include "include/fd.h"
#include "include/defs.h"
#include "log.h"
#include <algorithm>
#include <bits/types/struct_timeval.h>
#include <cstddef>
#include <cstdlib>
#include <fcntl.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unistd.h>

NT_NAMESPACE_BEGEN

file_discriptor::fd_wrapper::fd_wrapper(const value_type fd)
    : _fd(fd), _eof(false), _closed(false)
    , _read_count(0), _write_count(0) {
    if (_fd < 0 || !is_valid()) {
        fatal << "invalid file discriptor `" << _fd << "`";
        exit(1);
    }
}

file_discriptor::fd_wrapper::~fd_wrapper() {
    if (!_closed) close();
}

void file_discriptor::fd_wrapper::close() {
    ::close(_fd);
    _fd = -1;
}
bool file_discriptor::fd_wrapper::is_valid() const 
    { return fcntl(_fd, F_GETFD) != -1; }

file_discriptor::file_discriptor(const value_type fd)
    : _internal_fd(std::make_shared<fd_wrapper>(fd)) {}

file_discriptor::file_discriptor(std::shared_ptr<fd_wrapper> dup)
    : _internal_fd(std::move(dup)) {}

std::string file_discriptor::read(const value_type limit) {
    std::string buf;
    static_cast<void>(read(buf, limit));
    return buf;
}

ssize_t file_discriptor::read(std::string &buf, const value_type limit) {
    size_t buf_size = std::min<size_t>(1024 * 1024, limit);
    //! Previously, we neglected to allocate for `but_t`, resulting in a read failure
    void* but_t = (void*)malloc(buf_size);

    ssize_t read_len = ::read(get_fd(), but_t, buf_size);
    if (limit > 0 && read_len == 0) set_eof();
    if (read_len > static_cast<ssize_t>(buf_size)) {
        fatal << "read() read more than requested";
        return -1;
    }
    buf = static_cast<char*>(but_t);

    update_rd();

    return read_len;
}

ssize_t file_discriptor::write(const char* src, const value_type buf_len) {
    if (buf_len < 0) {
        exit(1);
    }
    ssize_t write_len = ::write(get_fd(), src, buf_len);

    return write_len;
}

ssize_t file_discriptor::write(const std::string& src, const value_type limit) {
    size_t write_size = limit <= src.size() ? limit : src.size();
    ssize_t write_len = ::write(get_fd(), src.data(), write_size);
    if (write_len < 0) {
        fatal << "write error!";
        _internal_fd->close();
        exit(1);
    }
    
    return write_len;
}

ssize_t file_discriptor::send(const char* src, const value_type buf_len) {
    return write(src, buf_len);
}
ssize_t file_discriptor::receive(std::string& buf, const value_type limit) {
    return read(buf, limit);
}
bool file_discriptor::is_readable() {
    return _internal_fd->_read_count > 0;
}
bool file_discriptor::is_writable() {
    return _internal_fd->_write_count < 1;
}
file_discriptor file_discriptor::duplicate() const {
    return file_discriptor(_internal_fd);
}
void file_discriptor::set_blocking() {
    int flag = ::fcntl(get_fd(), F_GETFL);
    //! There was a bug here earlier, but now it has been fixed
    if (!is_blocking()) {
        flag ^= (flag & O_NONBLOCK);
    } else {
        flag |= O_NONBLOCK;
    }
    ::fcntl(get_fd(), F_SETFL, flag);
}
void    file_discriptor::close()     { _internal_fd->close(); }

void    file_discriptor::update_rd() { _internal_fd->_read_count++; }
void    file_discriptor::update_wt() { _internal_fd->_write_count++; }
size_t  file_discriptor::get_fd()       const { return _internal_fd->_fd; }
void    file_discriptor::set_eof()            { _internal_fd->_eof = true; }
bool    file_discriptor::eof()          const { return _internal_fd->_eof; }
bool    file_discriptor::is_blocking()  const 
    { return !(::fcntl(get_fd(), F_GETFL) & O_NONBLOCK); }
bool    file_discriptor::is_closed()    const { return _internal_fd->_closed; }
ssize_t file_discriptor::get_rd_count() const { return _internal_fd->_read_count; }
ssize_t file_discriptor::get_wt_count() const { return _internal_fd->_write_count; }

NT_NAMESPACE_END