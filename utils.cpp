#include "utils.hpp"

Method resolveMethod(std::string const & word) {
    if(word == "GET" ) return MethodGET;
    if(word == "POST" ) return MethodPOST;
    if(word == "DELETE" ) return MethodDELETE;
    return MethodInvalid;
}

std::string toStr(Method method) {
    if(method == MethodGET ) return "GET";
    if(method == MethodPOST ) return "POST";
    if(method == MethodDELETE ) return "DELETE";
    return "";
}

HTTPVersion resolveHTTPVersion(std::string const & word) {
    if(word == "HTTP/1.0" ) return HTTPVer10;
    if(word == "HTTP/1.1" ) return HTTPVer11;
    return HTTPVerInvalid;
}

std::string toStr(HTTPVersion httpVersion) {
    if(httpVersion == HTTPVer10 ) return "HTTP/1.0";
    if(httpVersion == HTTPVer11 ) return "HTTP/1.1";
    return "";
}

StatusCode resolveStatusCode(std::string const & word) {
    if(word == "200" ) return StatusCode200;
    if(word == "404" ) return StatusCode404;
    return StatusCodeInvalid;
}

std::string toStr(StatusCode statusCode) {
    if(statusCode == StatusCode200 ) return "200";
    if(statusCode == StatusCode404 ) return "404";
    return "";
}

std::string strip(std::string input) {
    std::string::iterator first = input.begin();
    std::string::iterator last = input.end();

    for (std::string::iterator it = input.begin(); std::isspace(*it); ++it) {
        first++;
    }
    for (std::string::reverse_iterator it = input.rbegin(); std::isspace(*it); ++it) {
        last--;
    }
    return (first < last) ? std::string(first, last) : std::string();
}

std::string toLower(const std::string& input) {
    std::string result = input;
    for (std::size_t i = 0; i < result.length(); ++i) {
        result[i] = std::tolower(result[i]);
    }
    return result;
}

std::string toUpper(const std::string& input) {
    std::string result = input;
    for (std::size_t i = 0; i < result.length(); ++i) {
        result[i] = std::toupper(result[i]);
    }
    return result;
}

void truncateIfEndsWith(std::string& str, char c) {
    if (!str.empty() && str[str.length() - 1] == c) {
        str.erase(str.length() - 1);
    }
}

bool url_match_root(std::string str, std::string pattern) {
    truncateIfEndsWith(pattern, '/');

    return (toLower(str).substr(0, pattern.length()) == toLower(pattern) 
        && (str[pattern.length()] == '/' || toLower(str) == toLower(pattern)));
}

bool isValidDirectory(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        std::cerr << "Error accessing path: " << strerror(errno) << std::endl;
        return false;
    }
    return S_ISDIR(st.st_mode);
}

bool hasReadPermission(const std::string& path) {
    if (access(path.c_str(), R_OK) == 0) {
        return true; // Read permission is granted
    } else {
        std::cerr << "Error accessing path: " << strerror(errno) << std::endl;
        return false; // Read permission is not granted or an error occurred
    }
}

std::string extractFileName(const std::string& fullPath) {
    // Find the position of the last occurrence of the directory separator
    size_t lastSeparatorPos = fullPath.find_last_of("/\\");
    
    // If the separator is found, extract the substring after it (the file name)
    if (lastSeparatorPos != std::string::npos) {
        return fullPath.substr(lastSeparatorPos + 1);
    }
    
    // If no separator is found, return the full path as the file name
    return fullPath;
}

std::string extractExtension(const std::string& fileName) {
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos) {
        return fileName.substr(dotPos + 1);
    }
    return ""; // Return an empty string if no extension found
}

std::vector<std::string> listFilesInDirectory(const std::string& directoryPath) {
    std::vector<std::string> files;
    DIR *dir; // DIR = A type representing a directory stream.
    struct dirent *ent;
    if ((dir = opendir(directoryPath.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                files.push_back(ent->d_name);
            } else if (ent->d_type == DT_DIR) {
                if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                    files.push_back(ent->d_name);
                }
            }
        }
        closedir(dir);
    }

    return files;
}
