#include "User.h"
#include <string>
#include <iostream>
User::User(){

}
User::User(std::string FullnameArg, std::string AccountNameArg, std::string PasswordArg, int PointArg, std::string SaltArg, std::string WalletArg)
{

    this->Fullname = FullnameArg;
    this->AccountName = AccountNameArg;
    this->Password = PasswordArg;
    this->Point = PointArg;
    this->Salt = SaltArg;
    this->Wallet = WalletArg;

}


std::string User::salt()  {
    return this->Salt;
}
void User::setSalt( std::string &salt) {
    this->Salt = salt;
}

std::string User::fullName()  {
    return this->Fullname;
}
void User::setFullName( std::string &fullname) {
    this->Fullname = fullname;
}

std::string User::accountName()  {
    return this->AccountName;
}
void User::setAccountName( std::string &accountName) {
    this->AccountName = accountName;
}

std::string User::password()  {
    return this->Password;
}
void User::setPassword( std::string &password) {
    this->Password = password;
}

int User::point()  {
    return this->Point;
}
void User::setPoint(int point) {
    this->Point = point;
}

std::string User::wallet()  {
    return this->Wallet;
}
void User::setWallet(std::string &wallet) {
    this->Wallet = wallet;
}


User::~User()
{
}
