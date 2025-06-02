#ifndef USERSTATUS_H
#define USERSTATUS_H

#include <string>
#include <iostream>

// Định nghĩa các loại trạng thái người dùng
enum class UserStatusType {
    OK,
    DELETED,
    LOCKED,
    GENERATED_PASSWORD,
    ACTIVE
};

class UserStatus {
    private:
        std::string User;
        std::string isGeneratedPassword;
        std::string failedLogin;
        std::string deleteUser;
        std::string Date;

        void validateStatus();

    public:
        UserStatus(const std::string& userArg, 
                    const std::string& generatedPasswordArg, 
                    const std::string& failedStatusArg, 
                    const std::string& deleteUserStatusArg, 
                    const std::string& dateArg);
        ~UserStatus();
        
        std::string getUser() const;
        void setgetUser(const std::string& user);

        std::string getIsGeneratedPassword() const;
        void setIsGeneratedPassword(const std::string& generatedPassword);

        std::string getFailedLogin() const;
        void setFailedLogin(const std::string& status);

        std::string getDeleteUser() const;
        void setDeleteUser(const std::string& status);

        std::string getDate() const;
        void setDate(const std::string& date);

        // Phương thức tiện ích để chỉnh sửa trạng thái người dùng
        void markGenPassWordUserSet(); // Đặt isGeneratedPassword thành "true" 
        void lockAccount(); // Đặt failedLogin thành "true"
        void unlockAccount(); // Đặt failedLogin thành "false"
        void markAsDeleteUser(); // Đặt deleteUser thành "true"
        void restoreUser(); // Đặt deleteUser thành "false"
        
        // Hàm kiểm tra trạng thái người dùng
        UserStatusType checkStatus(const UserStatus& status) const {
            bool isGenerated = (status.getIsGeneratedPassword() == "true"); // Kiểm tra xem mật khẩu có được hệ thống sinh ra hay không
            bool isLocked = (status.getFailedLogin() == "true"); // Kiểm tra xem tài khoản có bị khóa do đăng nhập sai quá nhiều lần hay không
            bool isDeleted = (status.getDeleteUser() == "true"); // Kiểm tra xem tài khoản có bị xóa hay không
            if (isDeleted) {
                return UserStatusType::DELETED;
            } else if (isLocked) {
                return UserStatusType::LOCKED;
            } else if (isGenerated) {
                return UserStatusType::GENERATED_PASSWORD;
            }else {
                return UserStatusType::OK; // Trạng thái bình thường
            }
        }
};
 #endif // USERSTATUS_H