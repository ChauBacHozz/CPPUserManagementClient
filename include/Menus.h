#pragma once
#include "Client.h"
#include "User.h"
#include "Admin.h"
#include <DbUtils.h>

#include <iostream>
#ifndef MENUS_H
#define MENUS_H

void FirstLoginMenu();
void AdminLoginMenu(Client *& currentClient);
void UserLoginMenu(User *& currentUser);
void mainMenu(Client *& client);
bool isUserExist(std::string userName);
bool isvalisfullName(std::string fullname);
bool isvalidPassword(std::string password);
std::map<std::string, std::string> changeuserinfo(std::string& filename, User *& currentUser, bool isAdmin, bool forceChangePassword, bool isTargetAdmin);
void printchangeUserInfoMenu(bool isAdmin);


#endif