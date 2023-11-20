
#include "defs.h"
#include "fd.h"
#include <memory>
#include <sys/types.h>

NT_NAMESPACE_BEGEN
class socket {
    using limits = std::numeric_limits<ssize_t>;

  public:
    socket(const socket &) = delete;
    socket(socket &&) = delete;
    socket &operator=(const socket &) = delete;
    socket &operator=(socket &&) = delete;
    socket(std::string ip, short port);

    /**
     * @brief Read data from socket fd
     * @param buf The buffer to read the data into.
     * @param limit The maximum number of bytes to write.
     * @return the first of return value is the data of received from server,the
     * second of return value is number of bytes written.
     */
    ssize_t recv(std::string &buf, ssize_t limits = limits::max());

    /**
     * @brief Read data from socket fd
     * @param limit The maximum number of bytes to write.
     * @return the first of return value is the data of received from server,the
     * second of return value is number of bytes written.
     */
    std::pair<std::string, ssize_t> recv(ssize_t limits = limits::max());

    /**
     * @brief Send data through the file descriptor.
     *
     * @param src The data to be sent.
     * @param buf_len The length of the data.
     * @return The number of bytes sent.
     */
    ssize_t send(const char *content, const ssize_t buf_len);

    ssize_t send(std::string& content);

  private:
    std::unique_ptr<nt::file_discriptor> _fd;
};

NT_NAMESPACE_END