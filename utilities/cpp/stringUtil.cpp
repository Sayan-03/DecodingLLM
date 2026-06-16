#include <headers/StringUtil.hpp>

namespace StringUtil {
    std::vector<std::string> split(const std::string& text, char delimiter) {
        std::vector<std::string> result;
        size_t start = 0;
        size_t end = text.find(delimiter);

        while (end != std::string::npos) {
            // Extract the substring between the current positions
            result.push_back(text.substr(start, end - start));
            
            // Move past the delimiter character
            start = end + 1;
            end = text.find(delimiter, start);
        }
        
        // Add the remaining segment after the final delimiter
        result.push_back(text.substr(start));
        
        return result;
    }

    std::vector<std::string> split(const std::string& text, const std::string& delimiter) {
        std::vector<std::string> result;
        if (delimiter.empty()) {
            result.push_back(text);
            return result;
        }

        size_t start = 0;
        size_t end = text.find(delimiter);

        while (end != std::string::npos) {
            result.push_back(text.substr(start, end - start));
            start = end + delimiter.length();
            end = text.find(delimiter, start);
        }
        
        result.push_back(text.substr(start));
        return result;
    }
}