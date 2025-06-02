#include <iostream>
#include <string>
#include "JsonUtils.h"
#include <map>
#include <sstream>
#include <algorithm>
std::string escape_json(const std::string& s) {
    std::ostringstream o;
    for (auto c : s) {
        switch (c) {
            case '"': o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\b': o << "\\b"; break;
            case '\f': o << "\\f"; break;
            case '\n': o << "\\n"; break;
            case '\r': o << "\\r"; break;
            case '\t': o << "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    o << "\\u" << std::hex << (int)c;
                } else {
                    o << c;
                }
        }
    }
    return o.str();
}

std::string map_to_json(const std::map<std::string, std::string>& m) {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& pair : m) {
        if (!first) {
            oss << ",";
        }
        first = false;
        oss << "\"" << escape_json(pair.first) << "\":"
            << "\"" << escape_json(pair.second) << "\"";
    }
    oss << "}";
    return oss.str();
}
std::string trim(const std::string& str) {
    const std::string whitespace = " \t\n\r";
    size_t start = str.find_first_not_of(whitespace);
    size_t end = str.find_last_not_of(whitespace);
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// Parser rất đơn giản cho JSON kiểu { "key": "value", ... }
std::map<std::string, std::string> parse_simple_json(const std::string& input) {
    std::map<std::string, std::string> result;
    std::string json = input;

    // Loại bỏ dấu { và }
    json.erase(std::remove(json.begin(), json.end(), '{'), json.end());
    json.erase(std::remove(json.begin(), json.end(), '}'), json.end());

    std::stringstream ss(json);
    std::string pair;

    while (std::getline(ss, pair, ',')) {
        size_t colon = pair.find(':');
        if (colon == std::string::npos) continue;

        std::string key = trim(pair.substr(0, colon));
        std::string value = trim(pair.substr(colon + 1));

        // Loại bỏ dấu " nếu có
        if (key.front() == '"') key = key.substr(1, key.size() - 2);
        if (value.front() == '"') value = value.substr(1, value.size() - 2);

        result[key] = value;
    }

    return result;
}