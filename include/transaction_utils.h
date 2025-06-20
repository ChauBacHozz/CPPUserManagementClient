#ifndef TRANSACTION_UTILS_H
#define TRANSACTION_UTILS_H
#include "User.h"
#include "DbUtils.h"
#include "encrypt.h"
#include <iostream>
#include <random>
#include <iomanip>
#include <admin.h>
#include <chrono>
#include <fstream>
#include <ctime>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <string>

// ==== Hằng số sử dụng trong OTP/Giao dịch ====
const int OTP_LENGTH = 6;
const int OTP_VALIDITY_SECONDS = 15;
const int MAX_OTP_ATTEMPTS = 3;
const std::string SECRET_KEY_PREFIX = "SECURE_KEY_FOR_ACCOUNT_";
//const std::string LOG_FILE_NAME = "OTP.txt";



// ==== Các hàm tiện ích giao dịch và OTP ====

/**
 * Sinh một ID giao dịch ngẫu nhiên (10 chữ số).
 */
std::string generateTransactionID();

/**
 * Tạo mã OTP dựa trên khóa bí mật và ID giao dịch.
 */
std::string generateOTP(const std::string& secretKey, const std::string& transactionID);

/**
 * Kiểm tra mã OTP người dùng nhập có hợp lệ hay không.
 */
bool verifyOTP(const std::string& enteredOTP, const std::string& secretKey, const std::string& transactionID);

bool verifyOTPForUser(User* currentUser);

/**
 * Ghi log cho giao dịch vào file.
 */
// void logTransaction(int stt, const std::string& transactionID,
//                     const std::string& sourceAccount, const std::string& receiveAccount,
//                     const std::string& status);

/**
 * Hàm điều khiển toàn bộ phiên giao dịch.
 */
void runTransactionSession();

#endif // TRANSACTION_UTILS_H
