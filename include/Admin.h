#ifndef ADMIN_H
#define ADMIN_H

#include <string>
#include "User.h"
#include <DbUtils.h>
#include "Client.h"

class Admin : public Client
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

#endif