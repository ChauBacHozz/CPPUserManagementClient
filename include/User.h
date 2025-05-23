#pragma once
#include <string>

class User
{
private:
    std::string Fullname;
    std::string AccountName;
    std::string Password;
    std::string Salt;
    std::string Wallet;
    int Point = 0;
public:
    User();
    User(std::string FullnameArg, std::string AccountNameArg, std::string PasswordArg, int PointArg, std::string SaltArg, std::string WalletArg);
    ~User();


    std::string fullName() ;
    void setFullName( std::string &fullName);

    std::string accountName() ;
    void setAccountName( std::string &accountName);


    std::string password() ;
    void setPassword( std::string &password);


    int point() ;
    void setPoint(int point);

    std::string salt() ;
    void setSalt( std::string &fullname);

    std::string wallet() ;
    void setWallet( std::string &wallet);

};

