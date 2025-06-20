#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <random>
#include <fstream>
#include <ctime>
#include "transaction_utils.h"
#include "User.h"
#include "DbUtils.h"
#include "encrypt.h"
#include "admin.h"

//  const int OTP_LENGTH = 6;
//  const std::string SECRET_KEY_PREFIX = "SECURE_KEY_FOR_ACCOUNT_";
//  const int OTP_VALIDITY_SECONDS = 60;
//  const int MAX_OTP_ATTEMPTS = 3;
//  const std::string SAVE_LOG_FILE_NAME = "OTP.txt";

// ==== Các hàm tiện ích ====

std::string generateTransactionID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1000000000, 9999999999);
    return std::to_string(distrib(gen));
}

std::string generateOTP(const std::string& secretKey, const std::string& transactionID) {
    auto now = std::chrono::system_clock::now(); 
    long long currentTimeSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch().count();
    long long timeStep = currentTimeSeconds / OTP_VALIDITY_SECONDS; 

    std::string data = secretKey + std::to_string(timeStep) + transactionID; 
    std::hash<std::string> hasher; 
    size_t hashedData = hasher(data);

    std::stringstream ss; 
    ss << std::setw(OTP_LENGTH) << std::setfill('0') << (hashedData % static_cast<size_t>(pow(10, OTP_LENGTH)));
    return ss.str();
}

bool verifyOTP(const std::string& enteredOTP, const std::string& secretKey, const std::string& transactionID) {
    auto now = std::chrono::system_clock::now();
    long long currentTimeSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch().count();

    std::string generatedOTPNow = generateOTP(secretKey, transactionID);
    if (enteredOTP == generatedOTPNow) return true;

    long long previousTimeStep = (currentTimeSeconds - OTP_VALIDITY_SECONDS) / OTP_VALIDITY_SECONDS;
    std::string previousData = secretKey + std::to_string(previousTimeStep) + transactionID;
    std::hash<std::string> hasher;
    size_t hashedData = hasher(previousData);

    std::stringstream ss;
    ss << std::setw(OTP_LENGTH) << std::setfill('0') << (hashedData % static_cast<size_t>(pow(10, OTP_LENGTH)));
    return (enteredOTP == ss.str());
}

bool verifyOTPForUser(User* currentUser) {
    std::string secretKey = currentUser->salt();
    std::string transactionID = generateTransactionID();

    if(secretKey.empty() || transactionID.empty()) {
        std::cerr << "Error: Secret key or transaction ID is empty!" << std::endl;
        return false; // Exit if secret key or transaction ID is empty
    }

    int attempts = 0;
    const int maxAttempts = 3;
    while (attempts < maxAttempts) {
        std::string otp = generateOTP(secretKey, transactionID);
        std::cout << "OTP for user " << currentUser->accountName() << " is: " << otp << " (Valid for " << OTP_VALIDITY_SECONDS << " seconds)" << std::endl;

        std::string enteredOTP;
        std::cout << "Enter OTP to verify (Attempt " << (attempts + 1) << "/" << maxAttempts << "): ";
        std::cin >> enteredOTP;

        if (verifyOTP(enteredOTP, secretKey, transactionID)) {
            std::cout << "OTP verification successful!" << std::endl;
            return true; // OTP verified successfully
        } else {
            attempts++;
            std::cout << "Incorrect OTP. Please try again." << std::endl;
        }
    }
    std::cout << "You have entered incorrect OTP " << maxAttempts << " times. Verification failed." << std::endl;
    return false; // OTP verification failed after max attempts
}


// void logTransaction(int stt, const std::string& transactionID, const std::string& sourceAccount,
//                     const std::string& receiveAccount, const std::string& status) {
//     std::ofstream outFile(SAVE_LOG_FILE_NAME, std::ios_base::app);
//     if (outFile.is_open()) {
//         auto now = std::chrono::system_clock::now();
//         std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
//         std::string timeStr = std::ctime(&currentTime);
//         timeStr.pop_back(); // loại bỏ '\n'

//         outFile << stt << " - " << transactionID << " - "
//                 << sourceAccount << " - " << receiveAccount << " - "
//                 << timeStr << " - " << status << std::endl;
//         outFile.close();
//     } else {
//         std::cerr << "Khong the mo file log: " << SAVE_LOG_FILE_NAME << std::endl;
//     }
// }

// ==== Hàm để thực thi toàn bộ quá trình giao dịch ====

// void runTransactionSession() {
//     std::string accountNumber;
//     std::cout << "Nhap so tai khoan nguon (Source Account): ";
//     std::cin >> accountNumber;

//     std::string receiveAccount = "N/A";
//     std::string accountSpecificSecretKey = SECRET_KEY_PREFIX + accountNumber;

//     int transactionCounter = 0;

//     while (true) {
//         std::cout << "\n-----------------------------------" << std::endl;

//         transactionCounter++;
//         std::string transactionID = generateTransactionID();

//         std::cout << "Bat dau giao dich voi ID: " << transactionID << std::endl;

//         std::string otp = generateOTP(accountSpecificSecretKey, transactionID);
//         std::cout << "OTP cho giao dich " << transactionID << " la: " << otp << std::endl;
//         std::cout << "OTP nay co gia tri trong khoang " << OTP_VALIDITY_SECONDS << " giay." << std::endl;

//         int attempts = 0;
//         bool verified = false;
//         std::string logStatus = "OTP failed";

//         while (attempts < MAX_OTP_ATTEMPTS && !verified) {
//             std::string enteredOTP;
//             std::cout << "Nhap OTP de xac thuc giao dich " << transactionID
//                       << " (Lan thu " << attempts + 1 << "/" << MAX_OTP_ATTEMPTS << "): ";
//             std::cin >> enteredOTP;
//             attempts++;

//             if (verifyOTP(enteredOTP, accountSpecificSecretKey, transactionID)) {
//                 std::cout << "Xac thuc OTP thanh cong! Giao dich duoc phep thuc hien." << std::endl;
//                 verified = true;
//                 logStatus = "OTP success";
//             } else {
//                 if (attempts < MAX_OTP_ATTEMPTS) {
//                     std::cout << "OTP khong chinh xac. Vui long thu lai." << std::endl;
//                 } else {
//                     std::cout << "Ban da nhap sai OTP qua " << MAX_OTP_ATTEMPTS
//                               << " lan. Giao dich bi tu choi." << std::endl;
//                     logStatus = "OTP failed (Too many attempts)";
//                 }
//             }
//         }

//         logTransaction(transactionCounter, transactionID, accountNumber, receiveAccount, logStatus);
//         std::cout << "Giao dich da duoc ghi vao file log: " << SAVE_LOG_FILE_NAME << std::endl;

//         std::cout << "Ban co muon thuc hien giao dich khac khong? (y/n): ";
//         char continueChoice;
//         std::cin >> continueChoice;
//         if (continueChoice == 'n' || continueChoice == 'N') {
//             break;
//         }
//     }

//     std::cout << "Chuong trinh ket thuc." << std::endl;
// }

