#include "User.h"
#include <string>
#include <iostream>

User::User(std::string FullNameArg, std::string AccountNameArg, std::string PasswordArg, int PointArg, std::string SaltArg, std::string WalletArg)
{

    this->FullName = FullNameArg;
    this->AccountName = AccountNameArg;
    this->Password = PasswordArg;
    this->Point = PointArg;
    this->Salt = SaltArg;
    this->Wallet = WalletArg;

}


std::string User::salt() const {
    return this->Salt;
}
void User::setSalt(const std::string &salt) {
    this->Salt = salt;
}

std::string User::fullName() const {
    return this->FullName;
}
void User::setFullName(const std::string &fullName) {
    this->FullName = fullName;
}

std::string User::accountName() const {
    return this->AccountName;
}
void User::setAccountName(const std::string &accountName) {
    this->AccountName = accountName;
}

std::string User::password() const {
    return this->Password;
}
void User::setPassword(const std::string &password) {
    this->Password = password;
}

int User::point() const {
    return this->Point;
}
void User::setPoint(int point) {
    this->Point = point;
}

std::string User::wallet() const {
    return this->Wallet;
}
void User::setWallet(std::string wallet) {
    this->Wallet = wallet;
}


User::~User()
{
}
