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

Admin::Admin(std::string fullNameArg, std::string accountNameArg, std::string PasswordArg, int PointArg, std::string SaltArg, std::string WalletArg)
{
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


int Admin::point()  {
    return this->Point;
}
void Admin::setPoint(int point) {
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
