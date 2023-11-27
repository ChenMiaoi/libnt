#include "../defs.h"
#include <unordered_map>


NT_NAMESPACE_BEGEN
namespace HTTP {
  struct Response{
    std::string method;
    std::string path;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;    

    std::string to_string();
 };

  Response parse_response(std::string stream);
  Response parse_response(char* stream);
}
NT_NAMESPACE_END
