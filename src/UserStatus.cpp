#include "UserStatus.h"
#include <string>
#include <iostream>


UserStatus::UserStatus(const std::string& userArg, 
                       const std::string& genPassArg, 
                       const std::string& failloginArg, 
                       const std::string& deletArg,
                       const std::string& dateArg) {
    this->User = userArg;
    this->isGeneratedPassword = genPassArg;
    this->failedLogin = failloginArg;
    this->deleteUser = deletArg;
    this->Date = dateArg;
    validateStatus();
} 

std::string UserStatus::getUser() const {
    return this->User;
}
void UserStatus::setgetUser(const std::string& user) {
    this->User = user;
}

std::string UserStatus::getIsGeneratedPassword() const {
    return this->isGeneratedPassword;
}
void UserStatus::setIsGeneratedPassword(const std::string& status) {
    this->isGeneratedPassword = status;
    validateStatus();
}

std::string UserStatus::getFailedLogin() const {
    return this->failedLogin;
}
void UserStatus::setFailedLogin(const std::string& status) {
    this->failedLogin = status;
    validateStatus();
}

std::string UserStatus::getDeleteUser() const {
    return this->deleteUser;
}
void UserStatus::setDeleteUser(const std::string& status) {
    this->deleteUser = status;
    validateStatus();
}

std::string UserStatus::getDate() const {
    return this->Date;
}
void UserStatus::setDate(const std::string& date) {
    this->Date = date;
}

void UserStatus::markGenPassWordUserSet() {
    setIsGeneratedPassword(std::string("false"));
}

void UserStatus::lockAccount() {
    setFailedLogin(std::string("true"));
}

void UserStatus::unlockAccount() {
    setFailedLogin(std::string("false"));
}

void UserStatus::markAsDeleteUser() {
    setDeleteUser(std::string("true"));
}

void UserStatus::restoreUser() {
    setDeleteUser(std::string("false"));
}

void UserStatus::validateStatus() {
    // Here you can add validation logic if needed
    // For example, check if the status strings are not empty or follow a specific format
    if (User.empty() || isGeneratedPassword.empty() || failedLogin.empty() || deleteUser.empty() || Date.empty()) {
        std::cerr << "Invalid UserStatus: One or more fields are empty." << std::endl;
    }
    // Additional validation logic can be added here
    // For example, you might want to check if the date is in a valid format
    // or if the isGeneratedPassword, failedLogin, and deleteUser fields are set to "true" or "false"
    if (isGeneratedPassword != "true" && isGeneratedPassword != "false") {
        std::cerr << "Invalid isGeneratedPassword value: " << isGeneratedPassword << std::endl;
    }
    if (failedLogin != "true" && failedLogin != "false") {
        std::cerr << "Invalid failedLogin value: " << failedLogin << std::endl;
    }
    if (deleteUser != "true" && deleteUser != "false") {
        std::cerr << "Invalid deleteUser value: " << deleteUser << std::endl;
    }
}


UserStatus::~UserStatus()
{
    // Destructor logic if needed
    // Currently, no dynamic memory allocation is done in this class
}
