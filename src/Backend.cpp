#include "Backend.h"
#include <string>
#include <curl/curl.h>
#include "JsonUtils.h"
#include <map>
#include <iostream>
#include "json.hpp"
using json = nlohmann::json;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string sendAPI(json send_data) {
    CURL* curl = curl_easy_init();
    std::string result = "";
    if (curl) {
        std::string readBuffer;

        curl_easy_setopt(curl, CURLOPT_URL, "http://100.116.29.42:8080/login");


        std::string jsonData = send_data.dump();
        
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        else
            // std::cout << "Response: " << readBuffer << "\n";
            result = readBuffer;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return result;
}

std::string loginUserAPI(std::string& user_name, std::string& password) {
    std::string purpose = "USERLOGIN";
    json user_login_info = {
        {"purpose", purpose},
        {"username", user_name},
        {"password", password}
    };
    std::string api_result = sendAPI(user_login_info);
    return api_result;
}

std::string editUserAPI(json user_info, json updated_values) {
    std::string purpose = "USEREDIT";
    json user_edit_info;
    user_edit_info["purpose"] = purpose;
    user_edit_info["userinfo"] = user_info;
    user_edit_info["updatedvalues"] = updated_values;
    std::string api_result = sendAPI(user_edit_info);
    return api_result;
}

std::string registerUserAPI(json user_info){
    std::string purpose = "USERREGISTER";
    json user_register_info;
    user_register_info["purpose"] = purpose;
    user_register_info["userinfo"] = user_info;
    std::string api_result = sendAPI(user_register_info);
    return api_result;
}