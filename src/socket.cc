#include "include/socket.h"
#include "include/defs.h"
#include "include/log.h"
#include <arpa/inet.h>
#include <cstring>
#include <memory>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <utility>

NT_NAMESPACE_BEGEN
socket::socket(std::string ip, short port) {
    int ret = ::socket(AF_INET, SOCK_STREAM, 0);
    if (ret == -1) {
        fatal << "create socket error!";
        exit(1);
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    if (::connect(ret, (sockaddr *)&addr, sizeof(addr)) == -1) {
        fatal << "connect to server errror!";
        exit(2);
    }
    _fd = std::make_unique<file_discriptor>(ret);
}

ssize_t socket::recv(std::string &buf, ssize_t limits) {
    return _fd->read(buf, limits);
}
std::pair<std::string, ssize_t> socket::recv(ssize_t limits) {
    std::string res;
    ssize_t writted = _fd->receive(res, limits);
    return std::make_pair(res, writted);
}

ssize_t socket::send(const char *content, const ssize_t buf_len) {
    return _fd->send(content, buf_len);
}
ssize_t socket::send(std::string &content) {
    return send(content.c_str(), content.length());
}
NT_NAMESPACE_END