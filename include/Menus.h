#pragma once
#include "Client.h"
#include "User.h"
#include "Admin.h"

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
void changeuserinfo(std::string& filename, User *& currentUser, bool isAdmin, bool forceChangePassword);
void printchangeUserInfoMenu(bool isAdmin);


#endif