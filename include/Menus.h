#pragma once
#include "User.h"
#include "Admin.h"
#include <iostream>
#ifndef MENUS_H
#define MENUS_H

void FirstLoginMenu();
void AdminLoginMenu();
void UserLoginMenu(User *& currentUser);
void mainMenu();
bool isUserExist(std::string userName);
bool isvalisfullName(std::string fullname);
bool isvalidPassword(std::string password);
void changeuserinfo(std::string& filename, User *& currentUser, bool isAdmin);
void printchangeUserInfoMenu(bool isAdmin);


#endif