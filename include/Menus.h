#pragma once
#include "User.h"
#ifndef MENUS_H
#define MENUS_H

void FirstLoginMenu();
void AdminLoginMenu();
void listTransactions(User* currentUser,
                      std::string& username,
                      std::string& IDWallet,
                      std::string& startDate,
                      std::string& endDate,
                      bool isAdmin = false);
void UserLoginMenu(User *& currentUser);
void mainMenu();


#endif