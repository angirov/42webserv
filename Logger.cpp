#include "Logger.hpp"

Logger::Logger(LogLevel level, const std::string& filename, std::ostream& out)
    : outputStream(out), logLevel(level) {
    if (!filename.empty()) {
        logfile.open(filename.c_str(), std::ios::app);
    }
}

Logger::~Logger() {
    if (logfile.is_open()) {
        logfile.close();
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level >= logLevel) {
        time_t rawtime;
        struct tm * timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
        std::string timestamp(buffer);

        std::string levelString;
        switch (level) {
            case DEBUG: levelString = "DEBUG"; break;
            case INFO: levelString = "INFO"; break;
            case WARNING: levelString = "WARNING"; break;
            case ERROR: levelString = "ERROR"; break;
            default: levelString = "UNKNOWN"; break;
        }

        if (logfile.is_open()) {
            logfile << "[" << timestamp << "] [" << levelString << "]\t" << message << std::endl;
        }
        else {
            outputStream << "[" << timestamp << "] [" << levelString << "]\t" << message << std::endl;
        }
    }
}
