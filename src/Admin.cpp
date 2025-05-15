#include "User.h"
#include <string>
#include <iostream>
#include "arrow/io/file.h"
#include "parquet/stream_reader.h"
#include "DbUtils.h"
#include "Admin.h"
#include "Encrypt.h"
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
