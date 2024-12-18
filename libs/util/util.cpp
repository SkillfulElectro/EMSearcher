#include "util.h"

std::unordered_map<std::string, std::vector<std::string>> parseStringToMap(const std::string& input) {
    std::unordered_map<std::string, std::vector<std::string>> result;
    std::istringstream inputStream(input);
    std::string segment;

    // Split input by semicolon (;)
    while (std::getline(inputStream, segment, ';')) {
        // Trim whitespace at the start and end of the segment
        size_t start = segment.find_first_not_of(" \n\r\t");
        size_t end = segment.find_last_not_of(" \n\r\t");
        if (start == std::string::npos || end == std::string::npos) continue; // Skip empty segments
        segment = segment.substr(start, end - start + 1);

        // Find the position of the colon
        size_t colonPos = segment.find(':');
        if (colonPos == std::string::npos) continue; // Skip invalid lines

        // Extract the key and the value part
        std::string key = segment.substr(0, colonPos);
        std::string valuesPart = segment.substr(colonPos + 1);

        // Trim whitespace from key
        key.erase(0, key.find_first_not_of(" \n\r\t"));
        key.erase(key.find_last_not_of(" \n\r\t") + 1);

        // Parse the values (comma-separated)
        std::vector<std::string> values;
        std::istringstream valuesStream(valuesPart);
        std::string value;
        while (std::getline(valuesStream, value, ',')) {
            // Trim whitespace from each value
            size_t vStart = value.find_first_not_of(" \n\r\t");
            size_t vEnd = value.find_last_not_of(" \n\r\t");
            if (vStart != std::string::npos && vEnd != std::string::npos) {
                value = value.substr(vStart, vEnd - vStart + 1);
                values.push_back(value);
            }
        }

        // Add to the result map
        if (!key.empty() && !values.empty()) {
            result[key] = values;
        }
    }

    return result;
}

std::vector<std::string> getFilesInDirectory(const std::string &dirPath) {
    std::vector<std::string> filePaths;

    for (const auto &entry : std::filesystem::recursive_directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            filePaths.push_back(entry.path().string());
        }
    }

    return filePaths;
}

std::string getFileContent(const std::string &filePath) {
    std::ifstream file(filePath);
    if (file.is_open()) {
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        return content;
    }

    return std::string();
}

std::vector<std::string> getFileContents(std::vector<std::string> &filePaths) {
    std::vector<std::string> fileContents;

    for (const auto &filePath : filePaths) {
        std::ifstream file(filePath);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
            fileContents.push_back(content);
        }
    }

    return fileContents;
}

std::vector<std::string> split_word_index(const std::string &text, const std::unordered_set<char> &delimiters) {
    std::vector<std::string> word_index;
    std::string buffer;

    for (char c : text) {
        if (delimiters.find(c) == delimiters.end()) {
            buffer += c;
        } else if (!buffer.empty()) {
            word_index.push_back(buffer);
            buffer.clear();
        }
    }

    if (!buffer.empty() && buffer.length() > 1)
        word_index.push_back(buffer);

    return word_index;
}
