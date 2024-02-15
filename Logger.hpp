#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <ctime>

enum LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    UNKNOWN
};

class Logger {
private:
    std::ostream& outputStream;
    std::ofstream logfile;
    LogLevel logLevel;
    LogLevel stringToLogLevel(const std::string& levelStr);

public:
    Logger(std::string level, const std::string& filename = "", std::ostream& out = std::cout);
    Logger(LogLevel level, const std::string& filename = "", std::ostream& out = std::cout);
    ~Logger();
    void log(LogLevel level, const std::string& message);
};

#endif // LOGGER_H
