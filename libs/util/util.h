#ifndef EM_UTIL_GUARD
#define EM_UTIL_GUARD

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <unordered_map>

std::unordered_map<std::string, std::vector<std::string>> parseStringToMap(const std::string& input);
std::vector<std::string> getFilesInDirectory(const std::string &dirPath);
std::string getFileContent(const std::string &filePath);
std::vector<std::string> getFileContents(std::vector<std::string> &filePaths);
std::vector<std::string> split_word_index(const std::string &text, const std::unordered_set<char> &delimiters);

#endif // EM_UTIL_GUARD
