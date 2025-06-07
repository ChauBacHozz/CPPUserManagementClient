#ifndef ADMIN_H
#define ADMIN_H
#pragma once
#include <arrow/api.h>
#include <string>
#include "User.h"
#include "Client.h"

class Admin : public Client {
private:
    std::string FullName;
    std::string AccountName;
    std::string Password;
    std::string Salt;
    std::string Wallet;
    int Point = 0;
public:

    Admin();
    Admin(std::string fullNameArg, std::string accountNameArg, 
            std::string PasswordArg, int64_t PointArg, std::string SaltArg, 
            std::string WalletArg);
    ~Admin();

    std::string fullName();
    void setFullName(std::string &fullName);

    std::string accountName();
    void setAccountName(std::string accountName);


    std::string password();
    void setPassword(std::string password);

    int64_t point();
    void setPoint(int64_t point);
    
    std::string salt();
    void setSalt(std::string &salt);

    std::string wallet();
    void setWallet(std::string &wallet);
   

};

arrow::Status changeAdminName(std::string& filename, Admin* admin, 
                                std::string& newFullName);

arrow::Status changeAdminPassword(std::string& filename, Admin* admin,
                                    std::string& newPassword);

arrow::Status editAdminPoints(std::string& filename, Admin* admin, int64_t editPoints);

#endif