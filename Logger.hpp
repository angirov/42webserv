#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <ctime>

enum LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
private:
    std::ostream& outputStream;
    std::ofstream logfile;
    LogLevel logLevel;

public:
    Logger(LogLevel level, const std::string& filename = "", std::ostream& out = std::cout);
    ~Logger();
    void log(LogLevel level, const std::string& message);
};

#endif // LOGGER_H
