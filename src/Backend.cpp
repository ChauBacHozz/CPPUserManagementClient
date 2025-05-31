#include "Backend.h"
#include <string>
#include <curl/curl.h>
#include "JsonUtils.h"
#include <map>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

loginUser(std::string& user_name, std::string& password) {
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string readBuffer;

        curl_easy_setopt(curl, CURLOPT_URL, "http://100.116.29.42:8080/login");

        std::map send_data = {
            {"username", user_name},
            {"password", password}
        };
        std::string jsonData = map_to_json(send_data);
        
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
            std::cout << "Response: " << readBuffer << "\n";

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}