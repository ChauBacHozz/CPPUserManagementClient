#pragma once
#include <string>
#include "User.h"
class Admin
{
private:
    std::string AccountName = "admin";
    std::string Password = "admin";
public:
    Admin();
    
    ~Admin();


    std::string accountName();
    void setAccountName(std::string accountName);


    std::string password();
    void setPassword(std::string password);

    bool createUser(std::string FullName, std::string UserName, std::string UserPassword, int point);



};

