#include <iostream>
#include "User.h"
#include <string>

int main()
{
    std::string userName = "Nguyen Duc Phong";
    std::string userAccount = "Phong123";
    std::string userPassword = "ndPhongZ2222";
    int userPoint = 12;

    User tempUser(userName, userAccount, userPassword, userPoint);
    tempUser.printUserInfo();
    return 0;
}
