#pragma once
#include <string>
#include "json.hpp"
using json = nlohmann::json;

std::string loginUserAPI(std::string& user_name, std::string& password);
std::string editUserAPI(json user_info, json updated_values);
std::string registerUserAPI(json user_info);
std::string getUserInfoAllAPI();
std::string searchUserAPI(std::string userName);
std::string searchWalletAPI(std::string receiver_walletId, std::string receiver_fullname);
std::string senderUpdateWalletAPI(json user_info, int point);

