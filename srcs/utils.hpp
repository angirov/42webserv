#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>
#include <list>
#include <cctype> // for strip
#include <sys/stat.h>
#include <cstring> // for strerror, strcpy and strcat
#include <unistd.h> // for access()
#include <cstdlib> // For realpath
#include <dirent.h>
#include <fstream>
#include <ctime>

enum Method
{
    MethodInvalid,
    MethodGET,
    MethodPOST,
    MethodDELETE
};

enum HTTPVersion
{
    HTTPVerInvalid,
    HTTPVer10,
    HTTPVer11
};

enum StatusCode {
	StatusCodeInvalid,
	StatusCode200,
	StatusCode200dir,
	// StatusCode201,
	StatusCode301dir,
	// StatusCode400, // not understand
	StatusCode403,
	StatusCode404,
	StatusCode405,
	StatusCode413,
	StatusCode500,
	StatusCodePOST,
	StatusCodeDELETE,
	StatusCodePost500,
	StatusCodeCGI,
	StatusCode300 = 300,
	StatusCode301 = 301,
	StatusCode302 = 302,
	StatusCode303 = 303,
	StatusCode304 = 304,
	StatusCode305 = 305,
	StatusCode306 = 306,
	StatusCode307 = 307
};

enum ContentType
{
    ContentInvalid,
    ContentOctetStream,
    ContentPlain,
    ContentHTML,
    ContentCSS
};

Method resolveMethod(std::string const & word);
HTTPVersion resolveHTTPVersion(std::string const & word);
StatusCode resolveStatusCode(std::string const & word);

std::string toStr(Method method);
std::string toStr(HTTPVersion httpVersion);
std::string toStr(StatusCode statusCode);

std::string strip(std::string input);
std::string toLower(const std::string& input);
std::string toUpper(const std::string& input);

void truncateIfEndsWith(std::string& str, char c);
bool url_match_root(std::string str, std::string pattern);
bool isValidDirectory(const std::string& path);
bool hasReadPermission(const std::string& path);
bool hasWritePermission(const std::string& path);
bool isDirHasWritePermission(const std::string& path);
std::string extractFileName(const std::string& fullPath);
std::string extractExtension(const std::string& fileName);
std::vector<std::string> listFilesInDirectory(const std::string& directoryPath);
void writeStringToBinaryFile(const std::string& str, const std::string& filename);
std::string generateTimeStamp();
std::string getDifference(const std::string& first, const std::string& second);
std::string appendIfNotEndsWith(const std::string &str, char c);
std::string getDifference(const std::string& route, const std::string& url);
std::list<int> deductLists(const std::list<int>& list1, const std::list<int>& list2);
StatusCode resolveRedirectionStatusCode(int code);
size_t getHTTPBodySize(const std::string& httpResponse);
bool isInFirstLine(const std::string& input, const std::string& substring);
void removeAllElementsByValue(std::list<int>& lst, int value);

#endif
