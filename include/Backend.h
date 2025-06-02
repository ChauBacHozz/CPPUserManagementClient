#pragma once
#include <string>
#include "json.hpp"
using json = nlohmann::json;

std::string loginUserAPI(std::string& user_name, std::string& password);
std::string editUserAPI(json user_info, json updated_values);
