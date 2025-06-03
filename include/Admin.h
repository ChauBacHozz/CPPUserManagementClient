#ifndef ADMIN_H
#define ADMIN_H

#include <string>
#include "User.h"
#include <DbUtils.h>
#include "Client.h"

class Admin : public Client
{
private:
    std::string FullName;
    std::string AccountName;
    std::string Password;
    std::string Salt;
    std::string Wallet;
    int Point = 0;
public:

    Admin();
    Admin(std::string FullNameArg, std::string AccountNameArg, std::string PasswordArg, int PointArg, std::string SaltArg, std::string WalletArg);
    ~Admin();


    std::string fullName();
    void setFullName(std::string &fullName);

    std::string accountName();
    void setAccountName(std::string accountName);


    std::string password();
    void setPassword(std::string password);

    int point();
    void setPoint(int point);
    
    std::string salt();
    void setSalt(std::string &salt);

    std::string wallet();
    void setWallet(std::string &wallet);
   

};

#endif