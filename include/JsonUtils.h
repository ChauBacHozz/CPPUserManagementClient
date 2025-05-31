#pragma once
#include <string>
#include <map>
#include <sstream>

std::string escape_json(const std::string& s);
std::string map_to_json(const std::map<std::string, std::string>& m);

