#pragma once
#include <string>
#include <map>
#include <sstream>

std::string escape_json(const std::string& s);
std::string map_to_json(const std::map<std::string, std::string>& m);
std::string trim(const std::string& str);
std::map<std::string, std::string> parse_simple_json(const std::string& input);

