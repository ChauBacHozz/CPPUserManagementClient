#include "User.h"
#include <string>
#include <iostream>
#include <arrow/api.h>
#include "arrow/io/file.h"
#include "parquet/stream_reader.h"
#include "DbUtils.h"
#include "Admin.h"
#include "Encrypt.h"

Admin::Admin()
{

}

Admin::Admin(std::string fullNameArg, std::string accountNameArg, 
             std::string PasswordArg, int64_t PointArg, 
             std::string SaltArg, std::string WalletArg) {

    this->FullName = fullNameArg;
    this->AccountName = accountNameArg;
    this->Password = PasswordArg;
    this->Point = PointArg;
    this->Salt = SaltArg;
    this->Wallet = WalletArg;
}

std::string Admin::salt()  {
    return this->Salt;
}
void Admin::setSalt(std::string &salt) {
    this->Salt = salt;
}

std::string Admin::fullName()  {
    return this->FullName;
}
void Admin::setFullName( std::string &FullName) {
    this->FullName = FullName;
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


int64_t Admin::point()  {
    return this->Point;
}
void Admin::setPoint(int64_t point) {
    this->Point = point;
}

std::string Admin::wallet()  {
    return this->Wallet;
}
void Admin::setWallet(std::string &wallet) {
    this->Wallet = wallet;
}

Admin::~Admin()
{
    
}

//Đổi tên Admin
arrow::Status changeAdminName(std::string& filename, Admin* admin, 
                                std::string& newFullName) {
    std::map<std::string, std::string> updated_values = {{"FullName", newFullName}};
    arrow::Status status = updateAdminRow(filename, admin, updated_values);
    if (status.ok()) {
        admin->setFullName(const_cast<std::string&>(newFullName));
    }
    return status;
}

//Đổi mật khẩu
arrow::Status changeAdminPassword(std::string& filename, Admin* admin,
                                    std::string& newPassword) {
    std::string newSalt = generateSaltStr();
    if (newSalt.empty()) {
        std::cerr << "Error: Failed to generate salt!" << std::endl;
        return arrow::Status::UnknownError("Failed to generate salt");
    }
    // Hash mật khẩu mới với salt mới
    std::string hashedPassword = sha256(newPassword + newSalt);
    if (hashedPassword.empty()) {
        std::cerr << "Error: Failed to hash password!" << std::endl;
        return arrow::Status::UnknownError("Failed to hash password");
    }

    // Cập nhật cả Password và Salt vào file
    std::map<std::string, std::string> updated_values = {
        {"Password", hashedPassword},
        {"Salt", newSalt}
    };
    arrow::Status status = updateAdminRow(filename, admin, updated_values);
    if (!status.ok()) {
        std::cerr << "Error updating admin data: " << status.ToString() << std::endl;
        return status;
    }

    // Đồng bộ với đối tượng Admin
    admin->setPassword(const_cast<std::string&>(hashedPassword));
    admin->setSalt(const_cast<std::string&>(newSalt));

    return arrow::Status::OK();
}

//Hàm đổi điểm Admin-Ví tổng
arrow::Status editAdminPoints(std::string& filename, Admin* admin, int64_t editPoints) {
    int64_t currentPoints = admin->point();
    int64_t newPoints = currentPoints + editPoints;
    std::map<std::string, std::string> updated_values = {{"Points", std::to_string(newPoints)}};
    arrow::Status status = updateAdminRow(filename, admin, updated_values);
    if (status.ok()) {
        admin->setPoint(newPoints); // Cập nhật đối tượng Admin
    }
    return status;
}
