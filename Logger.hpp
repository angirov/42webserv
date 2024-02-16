#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib> // For getenv

enum LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    UNKNOWN
};

class Logger
{
private:
    std::string logfileName;
    std::ostream &outputStream;
    std::ofstream logfile;
    LogLevel loggerLevel;
    LogLevel stringToLogLevel(const std::string &levelStr);

public:
    Logger();
    Logger(std::string level, const std::string &filename = "", std::ostream &out = std::cout);
    Logger(LogLevel level, const std::string &filename = "", std::ostream &out = std::cout);
    Logger &operator=(const Logger &other);
    ~Logger();
    void log(LogLevel level, const std::string &message);
    void env(const char *var);

    int getLevel() { return loggerLevel; };
    static std::string str(int n);
};

#endif // LOGGER_H
