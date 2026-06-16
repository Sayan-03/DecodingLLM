#pragma once

#include <vector>
#include <string>

namespace StringUtil {
    std::vector<std::string> split(const std::string& text, char delimiter = ' ');
    std::vector<std::string> split(const std::string& text, const std::string& delimiter);
}