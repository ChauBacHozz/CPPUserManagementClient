#pragma once
#include <string>

class User
{
private:
    std::string FullName;
    std::string AccountName;
    std::string Password;
    std::string Salt;
    int Point = 0;
public:
    User(std::string FullNameArg, std::string AccountNameArg, std::string PasswordArg, int PointArg);
    
    ~User();


    std::string fullName() const;
    void setFullName(const std::string &fullName);

    std::string accountName() const;
    void setAccountName(const std::string &accountName);


    std::string password() const;
    void setPassword(const std::string &password);


    int point() const;
    void setPoint(int point);

    std::string salt() const;
    void setSalt(const std::string &fullName);

    void printUserInfo();


};

