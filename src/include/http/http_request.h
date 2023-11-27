#include "../defs.h"
#include <string>
#include <unordered_map>

NT_NAMESPACE_BEGEN
namespace HTTP {
  struct Request{
    std::string method;
    std::string path;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    std::string to_string();
  };
  Request parse_request(std::string stream);
  Request parse_request(char* stream);
}

NT_NAMESPACE_END

