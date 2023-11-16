#ifndef __NT_LOGGER_H
#define __NT_LOGGER_H

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>

constexpr const char* RESET   = "\033[0m";
constexpr const char* RED     = "\033[31m";
constexpr const char* GREEN   = "\033[32m";
constexpr const char* YELLOW  = "\033[33m";
constexpr const char* BLUE    = "\033[34m";
constexpr const char* MAGENTA = "\033[35m";
constexpr const char* CYAN    = "\033[36m";
constexpr const char* WHITE   = "\033[37m";

enum class LogLevel {
    INFO, DEBUG, WARN, ERRON, FATAL,
};

template <LogLevel Level>
static void Logger(const std::string& func, const std::string& file, int line, const std::string& msg) {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    auto time_format = std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");

    std::ostringstream ostream;
    ostream << time_format;

    std::string time_str = ostream.str();

    std::string level;
    const char* color;
    switch (Level) {
        case LogLevel::INFO:
            level = "INFO";
            color = GREEN;
            break;
        case LogLevel::DEBUG:
            level = "DEBUG";
            color = BLUE;
            break;
        case LogLevel::WARN:
            level = "WARN";
            color = YELLOW;
            break;
        case LogLevel::ERRON:
            level = "ERROR";
            color = RED;
            break;
        case LogLevel::FATAL:
            level = "FATAL";
            color = RED;
            break;
        }

    std::cout << color 
                << time_str
                << "[ " << level << "] "
                << "<" << file.substr(file.find_last_of("/") + 1) << ":" << line << "> "
                << func << ": "
                << msg
                << RESET << std::endl;
    // printf("\033[%dm%s [%5s] <%s:%d> %s: %s\033[0m\n", color,  
    //     time_str.c_str(), level.c_str(), file.substr(file.find_last_of("/") + 1).c_str(), line, func.c_str(), msg.c_str());    
}

template <LogLevel Level>
class Log {
public:
    Log(int line, const char* func, const char* file)
        : _line(line), _func(func), _file(file)  {}
    ~Log() { Logger<Level>(_func, _file, _line, _oss.str()); }

    template <typename T>
    Log& operator<< (const T& msg) {
        _oss << msg;
        return *this;
    }
private:
    int _line;
    std::string _func;
    std::string _file;
    std::ostringstream _oss;
};

#define info    Log<LogLevel::INFO> (__LINE__, __func__, __FILE__)
#define debug   Log<LogLevel::DEBUG> (__LINE__, __func__, __FILE__)
#define warn    Log<LogLevel::WARN> (__LINE__, __func__, __FILE__)
#define erron   Log<LogLevel::ERRON> (__LINE__, __func__, __FILE__)
#define fatal   Log<LogLevel::FATAL> (__LINE__, __func__, __FILE__)

#endif //! __NT_LOGGER_H