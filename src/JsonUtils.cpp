#include <iostream>
#include <string>
#include "JsonUtils.h"
#include <map>
#include <sstream>

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