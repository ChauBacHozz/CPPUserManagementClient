#pragma once
#include <string>

class User
{
private:
    std::string Name;    
    std::string AccountName;
    std::string Password;
    int Point = 0;
public:
    User(std::string NameArg, std::string AccountNameArg, std::string PasswordArg, int PointArg);
    
    ~User();

    std::string name() const;
    void setName(const std::string &name);


    std::string accountName() const;
    void setAccountName(const std::string &accountName);


    std::string password() const;
    void setPassword(const std::string &password);


    int point() const;
    void setPoint(int point);


    void printUserInfo();


};

