#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <random> // Để tạo ID giao dịch ngẫu nhiên

const int OTP_LENGTH = 6;
const std::string SECRET_KEY_PREFIX = "SECURE_KEY_FOR_ACCOUNT_"; // Prefix cho khóa bí mật
const int OTP_VALIDITY_SECONDS = 60; // giới hạn thời gian để nhập mã OTP
const int MAX_OTP_ATTEMPTS = 3; // Số lần thử nhập OTP tối đa

// Hàm tạo ID giao dịch ngẫu nhiên
std::string generateTransactionID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1000000000, 9999999999);
    return std::to_string(distrib(gen));}


// Hàm tạo OTP dựa trên thời gian, khóa bí mật và ID giao dịch
std::string generateOTP(const std::string& secretKey, const std::string& transactionID) {
    auto now = std::chrono::system_clock::now();
    long long currentTimeSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch().count();
    long long timeStep = currentTimeSeconds / OTP_VALIDITY_SECONDS;

    // Kết hợp khóa bí mật và bước thời gian và ID giao dịch
    std::string data = secretKey + std::to_string(timeStep) + transactionID;

    // Tạo một hash từ dữ liệu kết hợp
    std::hash<std::string> hasher;
    size_t hashedData = hasher(data);

    // Lấy một phần của hash và định dạng nó thành OTP có độ dài cố định
    std::stringstream ss;
    ss << std::setw(OTP_LENGTH) << std::setfill('0') << (hashedData % static_cast<size_t>(pow(10, OTP_LENGTH)));
    return ss.str();
}

// Hàm xác thực OTP
bool verifyOTP(const std::string& enteredOTP, const std::string& secretKey, const std::string& transactionID) {
    auto now = std::chrono::system_clock::now();
    long long currentTimeSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch().count();

    // Thử nghiệm với bước thời gian hiện tại
    std::string generatedOTPNow = generateOTP(secretKey, transactionID);
    if (enteredOTP == generatedOTPNow) {
        return true;
    }

    // Thử nghiệm với bước thời gian trước đó (để xử lý độ trễ nhỏ)
    std::string generatedOTPBefore = generateOTP(secretKey, transactionID);
    auto previousTime = now - std::chrono::seconds(OTP_VALIDITY_SECONDS);
    long long previousTimeSeconds = std::chrono::time_point_cast<std::chrono::seconds>(previousTime).time_since_epoch().count();
    long long previousTimeStep = previousTimeSeconds / OTP_VALIDITY_SECONDS;
    std::string generatedOTPPreviousStepData = secretKey + std::to_string(previousTimeStep) + transactionID;
    std::hash<std::string> previousStepHasher;
    size_t previousStepHashedData = previousStepHasher(generatedOTPPreviousStepData);
    std::stringstream previousStepSS;
    previousStepSS << std::setw(OTP_LENGTH) << std::setfill('0') << (previousStepHashedData % static_cast<size_t>(pow(10, OTP_LENGTH)));
    if (enteredOTP == previousStepSS.str()) {
        return true;
    }

    return false;
}

// int main() {
//     std::string accountNumber;
//     std::cout << "Nhap so tai khoan: ";
//     std::cin >> accountNumber;

//     std::string accountSpecificSecretKey = SECRET_KEY_PREFIX + accountNumber;

//     bool transactionSuccess = false;
//     bool tooManyAttempts = false;

//     while (!transactionSuccess && !tooManyAttempts) {
//         std::string transactionID = generateTransactionID();
//         std::cout << "Bat dau giao dich voi ID: " << transactionID << std::endl;

//         std::string otp = generateOTP(accountSpecificSecretKey, transactionID);
//         std::cout << "OTP cho giao dich " << transactionID << " la: " << otp << std::endl;
//         std::cout << "OTP nay co gia tri trong khoang " << OTP_VALIDITY_SECONDS << " giay." << std::endl;

//         int attempts = 0;
//         bool verified = false;

//         while (attempts < MAX_OTP_ATTEMPTS && !verified) {
//             std::string enteredOTP;
//             std::cout << "Nhap OTP de xac thuc giao dich " << transactionID << " (Lan thu " << attempts + 1 << "): ";
//             std::cin >> enteredOTP;
//             attempts++;

//             if (verifyOTP(enteredOTP, accountSpecificSecretKey, transactionID)) {
//                 std::cout << "Xac thuc OTP thanh cong cho giao dich " << transactionID << "! Giao dich duoc phep thuc hien." << std::endl;
//                 verified = true;
//                 transactionSuccess = true;
//             } else {
//                 if (attempts < MAX_OTP_ATTEMPTS) {
//                     std::cout << "OTP khong chinh xac. Vui long thu lai." << std::endl;
//                 } else {
//                     std::cout << "Ban da nhap sai OTP qua " << MAX_OTP_ATTEMPTS << " lan. Giao dich bi tu choi." << std::endl;
//                     tooManyAttempts = true; // THOÁT khỏi vòng lặp bên ngoài
//                 }
//             }
//         }
//     }
    
//     return 0;
// }
