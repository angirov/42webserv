#include "Logger.hpp"

Logger::Logger() : outputStream(std::cout) {}

Logger::Logger(LogLevel level, const std::string &logfileName, std::ostream &out)
    : logfileName(logfileName), outputStream(out), loggerLevel(level)
{
    if (!logfileName.empty())
    {
        logfile.open(logfileName.c_str(), std::ios::app);
    }
}

Logger::Logger(std::string level, const std::string &logfileName, std::ostream &out)
    : logfileName(logfileName), outputStream(out)
{
    loggerLevel = stringToLogLevel(level);
    if (!logfileName.empty())
    {
        logfile.open(logfileName.c_str(), std::ios::app);
    }
}

Logger &Logger::operator=(const Logger &other)
{
    if (this != &other)
    { // Avoid self-assignment
        // Close current logfile
        if (logfile.is_open())
        {
            logfile.close();
        }

        // Copy log level
        this->loggerLevel = other.loggerLevel;

        // Copy logfile stream
        if (other.logfile.is_open())
        {
            this->logfile.open(other.logfileName.c_str(), std::ios::app);
        }
    }
    return *this;
}

Logger::~Logger()
{
    if (logfile.is_open())
    {
        logfile.close();
    }
}

void Logger::log(LogLevel level, const std::string &message)
{
    if (level >= loggerLevel)
    {
        time_t rawtime;
        struct tm *timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
        std::string timestamp(buffer);

        std::string levelString;
        switch (level)
        {
        case DEBUG:
            levelString = "DEBUG";
            break;
        case INFO:
            levelString = "INFO";
            break;
        case WARNING:
            levelString = "WARNING";
            break;
        case ERROR:
            levelString = "ERROR";
            break;
        default:
            levelString = "UNKNOWN";
            break;
        }

        if (logfile.is_open())
        {
            logfile << "[" << timestamp << "] [" << levelString << "]\t" << message << std::endl;
        }
        else
        {
            outputStream << "[" << timestamp << "] [" << levelString << "]\t" << message << std::endl;
        }
    }
}

LogLevel Logger::stringToLogLevel(const std::string &levelStr)
{
    if (levelStr == "DEBUG")
    {
        return DEBUG;
    }
    else if (levelStr == "INFO")
    {
        return INFO;
    }
    else if (levelStr == "WARNING")
    {
        return WARNING;
    }
    else if (levelStr == "ERROR")
    {
        return ERROR;
    }
    else
    {
        return UNKNOWN;
    }
}

void Logger::env(const char *var)
{
    if (std::getenv(var))
        this->log(INFO, "Value of " + std::string(var) + " is " + std::string(std::getenv(var)));
    else
        this->log(INFO, "Value of " + std::string(var) + " is EMPTY");
}

std::string Logger::str(int n)
{
    std::stringstream ss;
    ss << n;
    return ss.str();
}

std::string Logger::str(double n)
{
    std::stringstream ss;
    ss << n;
    return ss.str();
}

std::string Logger::str(time_t n)
{
    std::stringstream ss;
    ss << n;
    return ss.str();
}
