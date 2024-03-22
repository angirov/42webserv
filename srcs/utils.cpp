/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwagner <mwagner@student.42wolfsburg.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/22 12:28:15 by mwagner           #+#    #+#             */
/*   Updated: 2024/03/22 12:28:16 by mwagner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
        std::cerr << "Error accessing path" << std::endl;
        return false;
    }
    return S_ISDIR(st.st_mode);
}

bool hasReadPermission(const std::string& path) {
    if (access(path.c_str(), R_OK) == 0) {
        return true; // Read permission is granted
    } else {
        std::cerr << "Error accessing path" << std::endl;
        return false; // Read permission is not granted or an error occurred
    }
}

bool hasWritePermission(const std::string& path) {
    if (access(path.c_str(), W_OK) == 0) {
        return true; // Read permission is granted
    } else {
        std::cerr << "Error accessing path" << std::endl;
        return false; // Read permission is not granted or an error occurred
    }
}

bool isDirHasWritePermission(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        // Error accessing file/directory
        return false;
    }
    if (S_ISDIR(info.st_mode)) {
        // Check if it's a directory
        if (access(path.c_str(), W_OK) == 0) {
            // Check if the process has writing permission
            return true;
        }
    }
    return false;
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

void writeStringToBinaryFile(const std::string &str, const std::string &filename)
{
    // Open the file in binary mode
    std::ofstream outfile(filename.c_str(), std::ios::out | std::ios::binary);

    if (!outfile)
    {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    // Write the string length as a 32-bit integer
    int32_t length = str.size();
    // outfile.write(reinterpret_cast<const char*>(&length), sizeof(length));

    // Write the string content
    outfile.write(str.data(), length);

    // Close the file
    outfile.close();
}

std::string generateTimeStamp()
{
    // Get the current time
    std::time_t rawtime;
    std::time(&rawtime);

    // Convert the current time to a string
    std::tm *timeinfo = std::localtime(&rawtime);
    char buffer[80];
    std::strftime(buffer, 80, "%Y%m%d%H%M%S", timeinfo);

    // Create a stringstream to build the file name
    std::stringstream filenameStream;
    filenameStream << buffer; // You can adjust the file extension as needed

    // Return the generated file name
    return filenameStream.str();
}

std::string appendIfNotEndsWith(const std::string &str, char c)
{
    if (str.empty() || str[str.length() - 1] != c)
    {
        return str + c;
    }
    else
        return str;
}

std::string getDifference(const std::string& route, const std::string& url) {
    std::string prefix  = appendIfNotEndsWith(route, '/');
    // Check if the url string starts with the prefix string
    if (url.compare(0, prefix.length(), prefix) == 0) {
        // If it does, return the part of the url string after the prefix string
        return (url.substr(prefix.length()));
    } else {
        // If not, return an empty string
        return "";
    }
}

std::list<int> deductLists(const std::list<int>& list1, const std::list<int>& list2) {
    std::list<int> resultList;
    for (std::list<int>::const_iterator it1 = list1.begin(); it1 != list1.end(); ++it1) {
        bool found = false;
        for (std::list<int>::const_iterator it2 = list2.begin(); it2 != list2.end(); ++it2) {
            if (*it1 == *it2) {
                found = true;
                break;
            }
        }
        if (!found) {
            resultList.push_back(*it1);
        }
    }
    return resultList;
}

StatusCode resolveRedirectionStatusCode(int code) {
	switch (code) {
		case 300: return StatusCode300;
		case 301: return StatusCode301;
		case 302: return StatusCode302;
		case 303: return StatusCode300;
		case 304: return StatusCode304;
		case 305: return StatusCode305;
		case 306: return StatusCode306;
		case 307: return StatusCode307;
		default:
			return StatusCode300; // Default to a safe redirection code
	}
}

size_t getHTTPBodySize(const std::string& httpResponse) {
    // Find the position of the double CRLF ("\r\n\r\n") that separates the header and body
    size_t bodyStart = httpResponse.find("\r\n\r\n");

    // If double CRLF is not found, return 0 (no body)
    if (bodyStart == std::string::npos)
        return 0;

    // Calculate the size of the body by subtracting the body start position
    // from the total length of the HTTP response
    return httpResponse.size() - (bodyStart + 4); // 4 is the length of "\r\n\r\n"
}

bool isInFirstLine(const std::string& input, const std::string& substring)
{
    // Find the position of the first newline character
    size_t pos = input.find('\n');
    
    // Extract the first line of the input string
    std::string firstLine = input.substr(0, pos);
    
    // Check if the substring exists in the first line
    if (firstLine.find(substring) != std::string::npos) {
        return true;
    }
    return false;
}

void removeAllElementsByValue(std::list<int>& lst, int value)
{
    std::list<int>::iterator it = lst.begin();
    while (it != lst.end()) {
        if (*it == value) {
            it = lst.erase(it);
        } else {
            ++it;
        }
    }
}
