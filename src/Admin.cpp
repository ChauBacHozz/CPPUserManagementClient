#include "Admin.h"
#include "User.h"
#include <string>
#include <iostream>
#include "arrow/io/file.h"
#include "parquet/stream_reader.h"
#include <cmath>
#include <iomanip>
#include <openssl/sha.h>
#include <random>
#include <sstream>
#include "DbUtils.h"
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

std::string generateSaltStr(int lenght = 16) {
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);


    std::string salt;
    for (int i = 0; i < lenght; i++)
    {
        salt += charset[dist(rng)];
    }
    return salt;
}

std::string sha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char *> (input.c_str()), input.length(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();    
}

bool Admin::createUser(std::string FullName, std::string UserName, std::string UserPassword, int point) {
    // Tạo mã hash từ toàn bộ thông tin người dùng
    std::string userBasicInfo = FullName + UserName + UserPassword;
    // Tạo mã hash cho userpassword
    std::string salt = generateSaltStr();
    std::string hashedPassword = sha256(UserPassword + salt);

    std::string walletId = sha256(hashedPassword + salt);

    // std::shared_ptr<arrow::io::ReadableFile> infile;

    // PARQUET_ASSIGN_OR_THROW(
    //     infile,
    //     arrow::io::ReadableFile::Open("../assets/users.parquet"));
  
    //  parquet::StreamReader stream{parquet::ParquetFileReader::Open(infile)};
  
    // std::string dbFullName;
    // std::string dbUserName;
    // std::string dbUserPassword;
    // std::string dbUserSalt;
    // int64_t dbUserPoint;
    // std::string dbWalletId;
    std::string filename = "../assets/users.parquet";
    arrow::Status status = AppendUserParquetRow(filename, FullName, UserName, hashedPassword, salt, point, walletId);




    return false;
}

Admin::~Admin()
{
}
