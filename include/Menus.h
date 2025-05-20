#pragma once
#include "User.h"
#ifndef MENUS_H
#define MENUS_H

void FirstLoginMenu();
void AdminLoginMenu();
void UserLoginMenu(User *& currentUser);
void mainMenu();


#endif