#include "Admin.h"
#include "User.h"
#include <string>
#include <iostream>
#include "arrow/io/file.h"
#include "parquet/stream_reader.h"
#include <cmath>
#include <iomanip>

Admin::Admin()
{

}


std::string Admin::accountName() {
    return this->AccountName;
}
void Admin::setAccountName(std::string accountName) {
    this->AccountName = accountName;

}

std::string Admin::password() {
    return this->Password;
}
void Admin::setPassword(std::string password) {
    this->Password = password;
}

std::string hashStringToHex(const std::string& input) {
    std::hash<std::string> hasher;
    size_t hash = hasher(input);

    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return ss.str();
}
bool Admin::createUser(std::string FullName, std::string UserName, std::string UserPassword, int point) {
    // Tạo mã hash từ toàn bộ thông tin người dùng
    std::string userBasicInfo = FullName + UserName + UserPassword;
    std::string walletId = hashStringToHex(userBasicInfo);

    std::cout << "Wallet ID: " << walletId << std::endl;




    return false;
}

Admin::~Admin()
{
}
