#include "Menus.h"
#include "Client.h"
#include "Admin.h"
#include "User.h"
#include "DbUtils.h"
#include <iostream>
#include "User.h"
#include <string>
#include "arrow/io/file.h"
#include "parquet/stream_reader.h"
#include "parquet/arrow/reader.h"
#include "parquet/arrow/writer.h"
#include "arrow/table.h"
#include <openssl/sha.h>
#include "DbUtils.h"
#include <encrypt.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <regex>
#include <transaction_utils.h>

using namespace std;

// Forward declaration for AppendUserStatusParquetRow
arrow::Status AppendUserStatusParquetRow(
    const std::string& statusFile,
    const std::string& userName,
    const std::string& isGeneratedPassword,
    const std::string& isFirstLogin,
    const std::string& isLocked,
    const std::string& lastLoginDate
);

void printAdminHomeMenu() {
    cout << "--- ADMIN HOME MENU ---" << endl;
    cout << "---------------------------------" << endl;
    cout << "1. User Listing" << endl;
    cout << "2. Manage User" << endl;
    cout << "3. Wallet Management" << endl;
    cout << "4. Editing Admin Info" << endl;
    cout << "0. Back" << endl;
    cout << "---------------------------------" << endl;
    cout << "Enter Your Option: ";
}

void printhistoryForAdmin() {
    cout << "--- HISTORY FOR ADMIN MENU ---" << endl;
    cout << "---------------------------------" << endl;
    cout << "1. History Transaction" << endl;
    cout << "2. History Edits Point" << endl;
    cout << "0. Back" << endl;
    cout << "---------------------------------" << endl;
    cout << "Enter Your Option: ";
}

void printAdminEditingInfo() {
        cout << "--- EDITING ADMIN INFO MENU ---\n";
        cout << "1. Change Full Name\n";
        cout << "2. Change Password\n";
        cout << "3. Edit Points\n";
        cout << "0. Back\n";
        cout << "-------------------------------\n";
        cout << "Enter your choice: ";
}

void printUserEditMenu() {
    cout << "--- ADMIN MANAGE USER MENU ---" << endl;
    cout << "---------------------------------" << endl;
    cout << "1. Add User" << endl;
    cout << "2. Add Users From CSV" << endl;
    cout << "3. Manage User Info" << endl;
    cout << "4. Deler Account" << endl;
    cout << "5. Restore Account" << endl;
    cout << "0. Back" << endl;
    cout << "---------------------------------" << endl;
    cout << "Enter your option: ";
}

void printAdminManageUserStatusMenu() {
    cout << "--- ADMIN MANAGE USER STATUS MENU ---" << endl;
    cout << "---------------------------------" << endl;
    cout << "1. Edit GenPassword Status" << endl;
    cout << "2. Unlock Account" << endl;
    cout << "3. Restore Account" << endl;
    cout << "0. Back" << endl;
    cout << "---------------------------------" << endl;
    cout << "Enter your option: ";
}

void printUserInfoFromDb(User *& currentUser) {
    cout << "\n--- USER INFO ---\n";
    cout << "---------------------------------\n";
    cout << "Full Name: " << currentUser->fullName() << endl;
    cout << "User Name: " << currentUser->accountName() << endl;
    cout << "--------------------------------\n" << endl;
}

// @param isAdmin 
void printchangeUserInfoMenu(bool isAdmin, bool isTargetAdmin) {
    //try{
    //cout << "Debudging change user info menu" << endl;
    cout << "\n--- CHANGE USER INFO MENU ---\n";
    cout << "---------------------------------\n";
    cout << "1. Change Full Name\n"; 
    if (isAdmin) {
        if(isTargetAdmin) { 
            cout << "2. Change Password\n";
        } else {
        cout << "2. Reset User Password\n";
        }
        cout << "3. Edit Point\n"; 
    } else {
        cout << "2. Change Password\n";
    }
    cout << "0. Back\n";
    cout << "---------------------------------\n";
    cout << "Enter your choice: ";
    //cout << "Debugging change user info menu end" << endl;
    //} catch (const std::exception& e) {
    //    cerr << "Error in printchangeUserInfoMenu: " << e.what() << endl;
    //}
}

void printeWalletMenu() {
    cout << "\n--- E-WALLET MENU ---\n" 
         << "1. Transfer Points\n" 
         << "2. Transaction History\n" 
         << "3. Transaction History by Time\n" 
         << "0. Back\n"
         << "---------------------------------\n"
         << "Enter your choice: ";
}

void printUserLoginMenu() {
    cout << "\n--- USER LOGIN MENU ---\n" 
         << "---------------------------------\n"
         << "1. Information\n" 
         << "2. E-Walelt\n"
         << "3. Delete Account\n"
         << "0. Back\n"
         << "---------------------------------\n"
         << "Enter your choice: ";
}

void printuserHomeMenu() {
    cout << "\n--- USER HOME MENU ---\n" 
         << "---------------------------------\n"
         << "1. Login\n" 
         << "2. Register\n" 
         << "0. Back\n"
         << "---------------------------------\n"
         << "Enter your choice: ";
}

void printmainMenu() {
    cout << "\n--- MAIN MENU ---\n" 
         << "---------------------------------\n"
         << "1. User Home\n" 
         << "2. Admin Home\n" 
         << "0. Exit\n"
         << "---------------------------------\n"
         << "Enter your choice: ";
}

bool isUserExist(std::string userName) {
    // Check if the user exists in the database
    // This is a placeholder function. You need to implement the actual logic.
    // For now, let's assume the user does not exist.
    std::shared_ptr<arrow::io::ReadableFile> infile;
    PARQUET_ASSIGN_OR_THROW(
        infile,
        arrow::io::ReadableFile::Open("../assets/users.parquet"));
    std::unique_ptr<parquet::arrow::FileReader> fileReader;
    PARQUET_ASSIGN_OR_THROW(fileReader, parquet::arrow::OpenFile(infile, arrow::default_memory_pool()));
    std::shared_ptr<arrow::Table> table;
    PARQUET_THROW_NOT_OK(fileReader->ReadTable(&table));
    auto column = table->GetColumnByName("UserName");
    if(!column) {
        std::cerr << "Column not found!" << std::endl;
        return false;
    }
    for(const auto& chunk : column->chunks()) {
        auto string_column = std::static_pointer_cast<arrow::StringArray>(chunk);
        for (int64_t i = 0; i < string_column->length(); i++) {
            if(!string_column->IsNull(i) && string_column->GetString(i) == userName) {
                return true; // User exists
            }
        }
    }
    return false; // User does not exist
}

bool isvalisfullName(std::string fullname) {
    fullname = trim(fullname);
    // Check if the full name is valid
    // This is a placeholder function. You need to implement the actual logic.
    // For now, let's assume the full name is valid.
    // if (fullname.length() < 5) {
    //     cout << "Full name is too short. Minimum length is 5 characters." << endl;
    //     return false;
    // }
    // else
 if(fullname.empty()) {
        cout << "Full name cannot be empty." << endl;
        return false;
    }
    // else if (fullname.length() > 50) {
    //     cout << "Full name is too long. Maximum length is 50 characters." << endl;
    //     return false;
    else { for(char c : fullname) {
        if(!isalpha(c) && c != ' ') {
            cout << "Full name can only contain letters and spaces." << endl;
            return false;
            }
        }
    }
    return true;
}

bool isvalidPassword(std::string password) {
    if(password.empty()) {
        cout << "Password cannot be empty." << endl;
        return false;
    }
    else if (password.length() < 8) {
        cout << "Password is easy. Requires at least 8 characters include numbers, upper lower letters, special characters!" << endl;
        return false;
    } 
    bool hasUpper = false,
         hasLower = false,
         hasDigit = false,
         hasSpecial = false;
    for (char c : password) {
        if (isupper(c)) hasUpper = true;
        else if (islower(c)) hasLower = true;
        else if (isdigit(c)) hasDigit = true;
        else if (ispunct(c)) hasSpecial = true;
    }
    if (!hasUpper || !hasLower || !hasDigit || !hasSpecial) {
        cout << "Password is easy. Requires at least 8 characters include numbers, upper lower letters, special characters!" << endl;
        return false;
    }
    
    // Nếu tất cả điều kiện được đáp ứng
    return true;
    
}

// User * enterUserInfo(){
//     system("cls");
//     string fullName;
//     string userName;
//     string password;
//     int point;

//     cout << "ENTER USER INFO" << endl;
//     cout << "---------------------------" <<endl;
//     cout << "User fullname: ";
//     //cin.ignore();
//     getline(cin, fullName);
//     cout << "User username: ";
//     getline (cin, userName);
//     cout << "User password: ";
//     getline (cin, password);
//     cout << "User user-point: ";
//     cin >> point;


//     User * user = new User();
//     return user;    
// }

//Hàm khai báo tạo user (gọi được ở cả menu Admin và user)
User * enterUserInfoRegister(bool isAdmin = false,  bool enterPoint = false){
    system("cls"); // Clear the console screen
    string FullName;
    string userName;
    string Password;
    //string genPassword;
    string hashedPassword;
    string genSalt;
    string genWalletId;
    int64_t userPoint = 0;
    genSalt = generateSaltStr(12); // Generate a random salt for the user

    cout << "ENTER USER INFO" << endl;
    cout << "---------------------------" <<endl;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore any leftover input
        
    while (true) {
        cout << "User FullName (or 'z' to return to Menu): ";
        getline(cin, FullName);
        FullName = trim(FullName);
        if (FullName == "z" || FullName == "Z") {
            cout << "User creation cancelled." << endl;
            cout << "Return to menu..." << endl;
            return nullptr; // Return to menu if user enters 'z'
        }
        if(!isvalisfullName(FullName)) {
        cout << "Invalid full name. Please try again." << endl;
        continue; // Prompt for full name again if invalid
        }
        break; // Exit the loop if the full name is valid
    }

    while (true){
        cout << "User username (or 'z' to return to Menu): ";
        getline (cin, userName);
        userName = trim(userName);
        if (userName == "z" || userName == "Z") {
            cout << "User creation cancelled." << endl;
            cout << "Return to menu..." << endl;
            return nullptr; // Return to menu if user enters 'z'
        }
        if (isUserExist(userName)) {
            cout << "Username already exists. Please try again." << endl;
        } 
        else {
            cout << "Username is valid." << endl;
            break; // Exit the loop if the username is unique
        }  
    }

    //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the newline character left in the input buffer
    bool isGeneratedPassword = false; // Flag to indicate if the password is generated
    while(true){
    if (isAdmin) {
        Password = generateSaltStr();
        hashedPassword = sha256(Password + genSalt);
        isGeneratedPassword = true; // Set flag to indicate password is generated
        cout << "Generated Password: " << Password << endl;
        cout << "Please note down this password for login!" << endl;
     if(hashedPassword.empty()) {
        cout << "Error hashing password." << endl;
        return nullptr;
        }
        break;
    } else {
        cout << "User password (leave empty to generate random password, or 'z' to return to menu): ";
        getline(cin, Password);
        Password = trim(Password);
        if (Password == "z" || Password == "Z") {
            cout << "User creation cancelled." << endl;
            cout << "Return to menu..." << endl;
            return nullptr; // Return to menu if user enters 'z'
        }
        if (isvalidPassword(Password)) {
            //cout << "Please try again!" << endl;
            continue;
        }
        if (Password.empty()) {
            // Generate a random password if the user leaves it empty or enters "z"
            Password = generateSaltStr(12);
            hashedPassword = sha256(Password + genSalt); 
            isGeneratedPassword = true; // Set flag to indicate password is generated
            cout << "Generated Password: " << Password << endl;
            cout << "Please note down this password for login!" << endl;
        } else {
            // Hash the provided password
            hashedPassword = sha256(Password + genSalt); 
            }
    }
    if (hashedPassword.empty()) {
                cout << "Error hashing password." << endl;
                cout << "Return to menu..." << endl;
                return nullptr; // Exit if password hashing fails
        }
    break;
    }
    
    if (isAdmin || enterPoint) {
        cout << "Enter user point: ";
        cin >> userPoint;
        cin.ignore();
        if (cin.fail() || userPoint < 0) {
            cin.clear(); // Clear the error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid point. Setting to 0" << endl;
            userPoint = 0; // Exit if point is invalid
        }
    }
    cout << "---------------------------" << endl;

    genWalletId = sha256(Password + genSalt);
    if (genWalletId.empty()) {
        cout << "Error generating wallet ID." << endl;
        cout << "Return to menu..." << endl;
        return nullptr; // Exit if wallet ID generation fails
    }

    // xác nhận trước khi tạo user
    string confirm;
    cout << "---------------------------" << endl;
    cout << "Comfirm user creation:" << endl;
    cout << "Full Name: " << FullName << endl;
    cout << "User Name: " << userName << endl;
    cout << "User Password: " << Password << endl;
    cout << "Are you sure to create this user? (Y/N): ";
    getline(cin, confirm);
    confirm = trim(confirm);
    // if (confirm == "z" || confirm == "Z") {
    //     cout << "User creation cancelled." << endl;
    //     cout << "Return to menu..." << endl;
    //     return nullptr; // Return to menu if user enters 'z'
    if (confirm != "Y" && confirm != "y") {
        cout << "User creation cancelled." << endl;
        cout << "Return to menu..." << endl;
        return nullptr; // Exit if user does not confirm
    }

    // Create a new User object with the provided information
User* user = new User(FullName, userName, hashedPassword, userPoint, genSalt, genWalletId);
if(!user) {
    cout << "Error creating user object." << endl;
    return nullptr; // Exit if user object creation fails
}
//ghi trạng thái generated password
if(isGeneratedPassword != false) {
    // Append user status to the parquet file
    // Assuming the function appendUserStatusRow is defined elsewhere
    // and it appends a row to the user status parquet file
cout << "Appending user status to parquet file..." << endl;
    // Define the status file and default values
std::string statusFile = "../assets/userstatus.parquet";
std::string defaultStatus = "false";
std::string defaultDate = "N/A"; // Hoặc có thể sử dụng ngày hiện tại
std::string isGenStr = isGeneratedPassword ? "true" : "false";
if(!appendUserStatusRow(statusFile, 
                            userName, 
                            isGenStr, 
                            defaultStatus, 
                            defaultStatus, 
                            defaultDate)) {
    cout << "Error appending user status to parquet file!" << endl;
    }
}
cout << "Debug: User object created successfully!" << endl;
// Return the created user object
return user;
}

// Edit user of Admin
bool UserEditMenu(Admin * currentAdmin) {
    int userEditMenuOption;
    do {
        system("cls");
        printUserEditMenu();
        cin >> userEditMenuOption;

    } while (userEditMenuOption < 0 || userEditMenuOption>5);
    switch (userEditMenuOption)
    {
        case 0: // back to Menu
            return true;
        case 1: { // tao usser moi
            // cout << "Add user" << endl; 
            // /* code */
            string saveParquetFileName = "../assets/users.parquet";

            // std::set<std::string> existingUsernames;
            // std::shared_ptr<arrow::io::ReadableFile> infile;
            // try {
            // PARQUET_ASSIGN_OR_THROW(infile, arrow::io::ReadableFile::Open(saveParquetFileName));
            // std::unique_ptr<parquet::ParquetFileReader> reader = parquet::ParquetFileReader::Open(infile);
            // //parquet::StreamReader stream(reader.get());
            // std::string dbFullName, dbUserName, dbHashedPassword, dbSalt, dbWalletId;
            // int64_t dbUserPoint;
            // while (!stream.eof()) {
            // stream >> dbFullName >> dbUserName >> dbHashedPassword >> dbSalt >> dbUserPoint >> dbWalletId >> parquet::EndRow;
            // existingUsernames.insert(dbUserName);
            // }
            // } catch (const arrow::Status& status) {
            // std::cerr << "Error reading file to check usernames: " << status.ToString() << std::endl;
            // std::cout << "Cannot proceed with user creation." << std::endl;
            // std::cin.get();
            // break;
            // }

            cin.clear(); // Clear the input buffer
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore any leftover input
            User * user = enterUserInfoRegister(true, true); // Pass true for isAdmin and enterPoint
            if (user == NULL)
            {
                cout << "Enter user info failed" << endl;
                cin.get(); // Wait for user input before continuing
                break;
            }

            std::string fullName = user->fullName();
            std::string accountName = user->accountName();
            std::string password = user->password();
            std::string salt = user->salt();
            int64_t point = user->point();
            std::string wallet = user->wallet();
        
            arrow::Status resultRegisterUser = AppendUserParquetRow(saveParquetFileName,
                                                                    fullName,
                                                                    accountName,
                                                                    password,
                                                                    salt,
                                                                    point,
                                                                    wallet);
            if (!resultRegisterUser.ok()) {
                std::cerr << "Error registering user: " << resultRegisterUser.ToString() << std::endl;
                cout << "User registration failed. Please try again." << endl;
                delete user; // Clean up the user object
                cin.get(); // Wait for user input before continuing
                break; // Exit the case if registration fails
            }
            
            std::cout << "User registered successfully!" << std::endl;
            delete user; // Clean up the user object
            cout << "Press Enter to continue..." << std::endl;
            cin.get(); // Wait for user input before continuing
            
        break;
        }

        case 2: { // add user theo lô (chưa hoàn thiện)
            std::cout << "DEBUG: Entering case 2 - Add Users From CSV" << std::endl;
            std::cout << "Add users from CSV" << std::endl;
            std::cout << "DEBUG: Exiting case 2" << std::endl;
            // string csvFileName;
            // cout << "Enter CSV file name (or 'z' to return to Menu): ";
            // cin.ignore();
            // getline(cin, csvFileName);
            // csvFileName = trim(csvFileName);
            // if (csvFileName == "z" || csvFileName == "Z") {
            //     cout << "Returning to Menu..." << endl;
            //     break;
            // }
            // arrow::Status result = addUsersFromCSV(csvFileName);
            // if (!result.ok()) {
            //     std::cerr << "Error adding users from CSV: " << result.ToString() << std::endl;
            // } else {
            //     std::cout << "Users added successfully!" << std::endl;
            // }
            // cin.get(); // Wait for user input before continuing
            break;
        }

        case 3: { //quản lý user
                string filename = "../assets/users.parquet";
                string userName;
                cout << "Enter user name for edit (or 'z' to return Menu): ";
                cin.ignore(numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
                getline (cin, userName);
                userName = trim(userName);
                if (userName == "z" || userName == "Z") {
                    cout << "Returning to Menu..." << std::endl;
                    break;
                }
                shared_ptr<arrow::io::ReadableFile> infile;
                try {
                    PARQUET_ASSIGN_OR_THROW(infile, arrow::io::ReadableFile::Open(filename));
                } catch (const arrow::Status& status) {
                    cerr << "Error opening file: " << status.ToString() << std::endl;
                    cout << "Press ENTER key to return to Menu..." << endl;
                    cin.get(); // Wait for user input before continuing
                    break;        
                }

                parquet::StreamReader stream{parquet::ParquetFileReader::Open(infile)};
                //parquet::StreamReader stream(reader.get());

                std::string dbFullName;
                std::string dbUserName;
                std::string dbSalt;
                std::string dbWalletId;
                std::string dbhasdedPassword;
                int64_t dbUserPoint;
                bool userfound = false;
                User * currentUser = nullptr;
 
                while (!stream.eof() ){
                    stream >> dbFullName >> dbUserName >> dbhasdedPassword >> dbSalt >> dbUserPoint >> dbWalletId >> parquet::EndRow;
                    //std::cout << dbUserName << std::endl;
                    if (userName == dbUserName) {
                        userfound = true; 
                        currentUser = new User(dbFullName, dbUserName, dbhasdedPassword, dbUserPoint, dbSalt, dbWalletId);
                        cout << "User found: " << currentUser->accountName() << endl;
                        break; 
                    }
                }
                if (!userfound || !currentUser) {
                    std::cout << "User " << userName <<" not found! (User invalid)" << std::endl;
                    cout << "Please check the user name and try again." << endl;
                    cout << "Press ENTER key to return to Menu..." << endl;
                    cin.get(); // Wait for user input before continuing
                    break;
                }
                // if (!currentUser) {
                //     cout << "User " << currentUser << " not found!" << endl;
                //     cout << "Please check the user name and try again." << endl;
                //     cin.get();
                //     break; // Skip to the next iteration of the loop
                // }
                // cout << "DEBUG: Current user found: " << currentUser->accountName() << ", calling changeuserinfo" << endl;
                auto updated_value = changeuserinfo(filename, currentUser, true, false, false); // Call the function to change user info
                //cout << "DEBUG: End of changeuserinfo" << endl
                if(!updated_value.empty()) {
                    //int64_t newPoints = dbUserPoint + int64_t.updated_value;
                    arrow::Status status = updateUserInfo(filename, currentUser, updated_value);
                    if(!status.ok()){
                        cout << "Error updating user info: " << status.ToString() << endl;
                    } else {
                        // Ghi log giao dịch chỉnh sửa điểm sau khi lưu thành công
                        if (updated_value.find("Points") != updated_value.end()) {
                            int64_t editpoints = stoll(updated_value["Points"]) - currentUser->point(); // Tính chênh lệch điểm
                            string reason;
                            cout << "Enter reason for point change: ";
                            getline(cin, reason);
                            reason = trim(reason);
                            if (reason.empty()) {
                                cout << "Reason cannot be empty. Setting to 'No reason provided'!" << endl;
                                reason = "No reason provided";
                            }
                            logPointEditTransaction(
                                currentAdmin->wallet(),          // Wallet ID của admin
                                currentAdmin->accountName(),     // Username của admin
                                currentAdmin->fullName(),        // Full name của admin
                                currentUser->wallet(),           // Wallet ID của user
                                currentUser->accountName(),      // Username của user
                                currentUser->fullName(),         // Full name của user
                                editpoints,                      // Số điểm thay đổi
                                true,                            // Giả định thành công
                                reason                           // Lý do
                            );
                            cout << "Transaction logged successfully!" << endl;
                        }
                        cout << "All changes saved successfully!" << endl;
                        }
                    }
                cout << "Press ENTER key to return to Menu..." << endl;
                cin.get(); // Wait for user input before continuing 
                delete currentUser;
                currentUser = nullptr;
            break;
        }
        case 4: { //delete account
            string fileStatus = "../assets/userstatus.parquet";
            string user;
            cout << "Enter user name for delete (or 'z' to return Menu): ";
                cin.ignore(numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
                getline (cin, user);
                user = trim(user);
                if (user == "z" || user == "Z") {
                    std::cout << "Returning to Menu..." << std::endl;
                    //string currentUser = nullptr;
                    break;
                }
                std::shared_ptr<arrow::io::ReadableFile> infileStatus;
                try {
                    PARQUET_ASSIGN_OR_THROW(infileStatus, arrow::io::ReadableFile::Open(fileStatus));
                } catch (const arrow::Status& status) {
                    std::cerr << "Error opening file: " << status.ToString() << std::endl;
                    cout << "Returning to Menu..." << endl;
                    std::cin.get(); // Wait for user input before continuing
                    break;        
                }

                parquet::StreamReader stream{parquet::ParquetFileReader::Open(infileStatus)};
                //parquet::StreamReader stream(reader.get());

                std::string dbUser, dbIsGeneratedPassword, dbIsFirstLogin, dbIsLocked, dbLastLoginDate;
                bool userFound = false;

                while (!stream.eof()) {
                    stream >> dbUser >> dbIsGeneratedPassword >> dbIsFirstLogin >> dbIsLocked >> dbLastLoginDate >> parquet::EndRow;
                    if (user == dbUser) {
                        userFound = true;
                        cout << "User Status Info:" << endl;
                        cout << "UserName: " << dbUser << endl;
                        cout << "isGeneratedPassword: " << dbIsGeneratedPassword << endl;
                        cout << "isFailedLogin: " << dbIsFirstLogin << endl;
                        cout << "isDeleted: " << dbIsLocked << endl;
                        cout << "LastLoginDate: " << dbLastLoginDate << endl;
                        break;
                    }
                }
                
                if (!userFound) {
                    cout << "User status not found for: " << user << endl;
                    cin.get();
                    break;
                }
                string confirm;
                cout << "Are you sure to delete this account? (Y/N)" << endl;
                getline(cin, confirm);
                confirm = trim(confirm);
                if(confirm == "N" || confirm == "n") {
                    cout << "Delete account canceled!" << endl;
                    break;
                }
                map<string, string> update_values = {{"deleteUser", "true"}};
                if(updateUserStatusRow(fileStatus, user, update_values)) {
                cout << "Account was deleted!" << endl;
                } else {
                cout << "Account was not deleted, Please check system!" << endl;
                }
                cout << "Press ENTER key to continue..." << endl;
                cin.get();
                break;
        }
        case 5: { //restore account
            string fileStatus = "../assets/userstatus.parquet";
            string user;
            cout << "Enter user name for edit (or 'z' to return Menu): ";
                cin.ignore(numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
                getline (cin, user);
                user = trim(user);
                if (user == "z" || user == "Z") {
                    std::cout << "Returning to Menu..." << std::endl;
                    //string currentUser = nullptr;
                    break;
                }
                std::shared_ptr<arrow::io::ReadableFile> infileStatus;
                try {
                    PARQUET_ASSIGN_OR_THROW(infileStatus, arrow::io::ReadableFile::Open(fileStatus));
                } catch (const arrow::Status& status) {
                    std::cerr << "Error opening file: " << status.ToString() << std::endl;
                    cout << "Returning to Menu..." << endl;
                    std::cin.get(); // Wait for user input before continuing
                    break;        
                }

                parquet::StreamReader stream{parquet::ParquetFileReader::Open(infileStatus)};
                //parquet::StreamReader stream(reader.get());

                std::string dbUser, dbIsGeneratedPassword, dbIsFirstLogin, dbIsLocked, dbLastLoginDate;
                bool userFound = false;

                while (!stream.eof()) {
                    stream >> dbUser >> dbIsGeneratedPassword >> dbIsFirstLogin >> dbIsLocked >> dbLastLoginDate >> parquet::EndRow;
                    if (user == dbUser) {
                        userFound = true;
                        cout << "User Status Info:" << endl;
                        cout << "UserName: " << dbUser << endl;
                        cout << "isGeneratedPassword: " << dbIsGeneratedPassword << endl;
                        cout << "isfailedLogin: " << dbIsFirstLogin << endl;
                        cout << "isDeleted: " << dbIsLocked << endl;
                        cout << "LastLoginDate: " << dbLastLoginDate << endl;
                        break;
                    }
                }
                
                if (!userFound) {
                    cout << "User status not found for: " << user << endl;
                    cin.get();
                    break;
                }
                string confirm;
                cout << "Are you sure to restore this account? (Y/N)" << endl;
                getline(cin, confirm);
                confirm = trim(confirm);
                if(confirm == "N" || confirm == "n") {
                    cout << "Restore account canceled!" << endl;
                    break;
                }
                map<string, string> update_values = {{"deleteUser", "false"}, {"failedLogin", "false"}};
                if(updateUserStatusRow(fileStatus, user, update_values)) {
                cout << "Account was restore. The secon step you need reset password for user: " << dbUser << "!" << endl;
                //reset password for user
                string filename = "../assets/users.parquet";
                shared_ptr<arrow::io::ReadableFile> infile;
                try {
                    PARQUET_ASSIGN_OR_THROW(infile, arrow::io::ReadableFile::Open(filename));
                } catch (const arrow::Status& status) {
                    cerr << "Error opening file: " << status.ToString() << std::endl;
                    cout << "Press ENTER key to return to Menu..." << endl;
                    cin.get(); // Wait for user input before continuing
                    break;        
                }

                parquet::StreamReader stream{parquet::ParquetFileReader::Open(infile)};
                //parquet::StreamReader stream(reader.get());

                std::string dbFullName;
                std::string dbUserName;
                std::string dbSalt;
                std::string dbWalletId;
                std::string dbhasdedPassword;
                int64_t dbUserPoint;
                bool userfound = false;
                User * currentUser = nullptr;
 
                while (!stream.eof() ){
                    stream >> dbFullName >> dbUserName >> dbhasdedPassword >> dbSalt >> dbUserPoint >> dbWalletId >> parquet::EndRow;
                    //std::cout << dbUserName << std::endl;
                    if (user == dbUserName) {
                        userfound = true; 
                        currentUser = new User(dbFullName, dbUserName, dbhasdedPassword, dbUserPoint, dbSalt, dbWalletId);
                        cout << "User found: " << currentUser->accountName() << endl;
                        break; 
                    }
                }
                if (!userfound || !currentUser) {
                    std::cout << "User " << user <<" not found! (User invalid)" << std::endl;
                    cout << "Please check the user name and try again." << endl;
                    cout << "Press ENTER key to return to Menu..." << endl;
                    cin.get(); // Wait for user input before continuing
                    break;
                }
                // if (!currentUser) {
                //     cout << "User " << currentUser << " not found!" << endl;
                //     cout << "Please check the user name and try again." << endl;
                //     cin.get();
                //     break; // Skip to the next iteration of the loop
                // }
                // cout << "DEBUG: Current user found: " << currentUser->accountName() << ", calling changeuserinfo" << endl;
                auto updated_value = changeuserinfo(filename, currentUser, true, false, false); // Call the function to change user info
                //cout << "DEBUG: End of changeuserinfo" << endl
                if(!updated_value.empty()) {
                    //int64_t newPoints = dbUserPoint + int64_t.updated_value;
                    arrow::Status status = updateUserInfo(filename, currentUser, updated_value);
                    if(!status.ok()){
                        cout << "Error updating user info: " << status.ToString() << endl;
                    } else {
                        cout << "All changes saved successfully!" << endl;
                    }
            }

                } else {
                cout << "Account was not restore, Please check system!" << endl;
                }
                cout << "Press ENTER key to continue..." << endl;
                cin.get();
                break;
        }
        default:
        break;
    }

    return false;
}

// bool loginAdmin(Admin *& currentAdmin) {
//     string adminName;
//     string adminPassword;
//     // string userName;
//     cout << "User admin: ";
//     cin >> adminName;
//     cout << "Password admin: ";
//     cin >> adminPassword;

//     if (adminName == "admin" && adminPassword == "admin") {
//         // currentAdmin->setAccountName(adminName);
//         // currentAdmin->setPassword(adminPassword);
//         return true;
//     } else {
//         return false;
//     }
//     // return false;
// }

void AdminLoginMenu(Client *& currentClient) {
    Admin* currentAdmin = nullptr;
    
    shared_ptr<arrow::io::ReadableFile> infile;

    loginAdmin(infile, currentAdmin);
        
    if (currentAdmin != nullptr) {
        currentClient = currentAdmin;
        bool adminLoginMenuExit = false;
        while (!adminLoginMenuExit) {
            system("cls");
            int adminHomeMenuOption;
            do {
                system("cls");
                printAdminHomeMenu();
                cin >> adminHomeMenuOption;
                //cout << "Your cho " << adminHomeMenuOption << endl;
    
            } while (adminHomeMenuOption < 0 || adminHomeMenuOption>4);
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            switch (adminHomeMenuOption) {
            case 0:
                cout << "Returning to main menu...\n";
                adminLoginMenuExit = true;
                break;
            case 1: { //Liệt kê danh sách user
                system("cls");
                bool exit = false;
                do
                {
                    arrow::Status status = printUserInfoFromDb();
                    if (!status.ok()) {
                        std::cerr << "Error printing user info: " << status.ToString() << std::endl;
                        break;
                    }
                    
                    std::cout << "Enter Z to go back: ";
                    std::string exit_char;
                    std::cin >> exit_char;
                    if (exit_char == "Z" || exit_char == "z") {
                        exit = true;
                    }
                    /* code */
                } while (!exit);
            }
                /* code */
                break;

            case 2: { // chuyển vào menu Manage user
                system("cls");
                bool exit = false;
                do
                {
                    bool exitUserEditMenu = UserEditMenu(currentAdmin);
                    if (exitUserEditMenu) {
                        exit = true;
                    }
                    /* code */
                } while (!exit);
                // currentAdmin->createUser(user1->fullname(), user1->accountName(), user1->password(), user1->point());
                break;
            }
    
            case 3: { //quản lý ví tổng
                while (true){
                int subChoice;
                system("cls");
                cout << "---- Check Balance ----\n";
                cout << "-----------------------\n";
                int64_t walletsystem = currentAdmin->point();
                cout << "\nWallet system: " << walletsystem << endl;
                std::string filename = "../assets/users.parquet";
                int64_t total = calculateTotalWalletIds(filename);
                //cout << "Total Wallet all User: " << total << endl;
                if (walletsystem == total) {
                cout << "Status: Wallet system matches Total Wallet all User (Balanced).\n";
                } else {
                    cout << "Status: Wallet system does NOT match Total Wallet all User. Please check history!\n";
                }
                cout << "\n-------------------------\n";
                printhistoryForAdmin();
                cin >> subChoice;
                cin.ignore(); // Ignore the newline character left in the input buffer
                if(subChoice==1){
                    string username, startDate, endDate;
                    cout << "Enter username to filter (or 'All' for all users, or leave blank): ";
                    getline(cin, username);
                    username = trim(username);
                    cout << "Enter start date (DD/MM/YYYY, or leave blank): ";
                    getline(cin, startDate);
                    startDate = trim(startDate);
                    cout << "Enter end date (DD/MM/YYYY, or leave blank): ";
                    getline(cin, endDate);
                    endDate = trim(endDate);
                    listTransactions(nullptr, username.empty() ? "All" : username, "", startDate, endDate, true, "Transfer");
                    cout << "Press ENTER to continue...";
                    cin.get();
                    break;
                }else if(subChoice==2) {
                    string username, startDate, endDate;
                    cout << "Enter username to filter (or 'All' for all users, or leave blank): ";
                    getline(cin, username);
                    username = trim(username);
                    cout << "Enter start date (DD/MM/YYYY, or leave blank): ";
                    getline(cin, startDate);
                    startDate = trim(startDate);
                    cout << "Enter end date (DD/MM/YYYY, or leave blank): ";
                    getline(cin, endDate);
                    endDate = trim(endDate);
                    listPointEdits(nullptr, username.empty() ? "All" : username, "", startDate, endDate, true, "PointEdit");
                    cout << "Press ENTER to continue...";
                    cin.get();
                    break;
                } else if(subChoice == 0) {
                    cout << "Returning menu ...." << endl;
                    break;
                }

                cout << "\nPress ENTER to continue...";
                cin.get();
                break;
            }
            break;
            }

            case 4: { //Thay đổi thông tin user admin
                //printUserInfoFromDb(currentAdmin);
                std::string filename = "../assets/admin.parquet";
                //cout << "DEBUG: End of changeuserinfo" << endl;
                bool adminEditingMenuExit = false;
                do {
                printAdminEditingInfo();
                int choice;
                cin >> choice;
                cin.ignore();
                string newValue;
                int64_t newPoints;
                switch (choice) {
                    case 1: {
                        cout << "Enter new full name (or 'z' to return menu): ";
                        getline(cin, newValue);
                        newValue = trim(newValue);
                        if (newValue == "z" || newValue == "Z") {
                        std::cout << "Returning to Menu..." << std::endl;
                        // currentAdmin = nullptr;
                        // adminEditingMenuExit = true;
                        break;
                        }
                        if (changeAdminName(filename, currentAdmin, newValue).ok()) {
                            cout << "Name changed successfully!\n";
                        } else {
                            cout << "Failed to change name!\n";
                        }
                        cout << "Press ENTER to continue...";
                        cin.get();
                        break; // Thoát để quay lại menu cha
                    }
                    case 2: {
                        cout << "Enter new password (or 'z' to return menu): ";
                        getline(cin, newValue);
                        newValue = trim(newValue);
                        if (newValue == "z" || newValue == "Z") {
                        std::cout << "Returning to Menu..." << std::endl;
                        // currentAdmin = nullptr;
                        // adminEditingMenuExit = true;
                        break;
                        }
                        if (changeAdminPassword(filename, currentAdmin, newValue).ok()) {
                            cout << "Password changed successfully!\n";
                        } else {
                            cout << "Failed to change password!\n";
                        }
                        cout << "Press ENTER to continue...";
                        cin.get();
                        break; // Thoát để quay lại menu cha
                    }
                    case 3: {
                        string reason;
                        cout << "Enter reason for point change: ";
                        getline(cin, reason);
                        reason = trim(reason);
                        if (reason.empty()) {
                            cout << "Reason cannot be empty. Setting to 'No reason provided'!" << endl;
                            reason = "No reason provided";
                        }
                        cout << "Enter edit points (or 'z' to return menu): ";
                        getline(cin, newValue);
                        newValue = trim(newValue);
                        if (newValue == "z" || newValue == "Z") {
                        std::cout << "Returning to Menu..." << std::endl;
                        // currentAdmin = nullptr;
                        // adminEditingMenuExit = true;
                        break;
                        }
                        int64_t editPoints = 0;
                        try {
                            editPoints = stoll(newValue);
                        } catch (...) {
                            cout << "Invalid point. Setting to 0!" << endl;
                            editPoints = 0;
                        }
                        if (editAdminPoints(filename, currentAdmin, editPoints).ok()) {
                            logPointEditTransaction(currentAdmin->wallet(),
                                                    currentAdmin->accountName(),
                                                    currentAdmin->fullName(),
                                                    currentAdmin->wallet(),
                                                    currentAdmin->accountName(),
                                                    currentAdmin->fullName(),
                                                    editPoints,
                                                    true, reason);
                            cout << "Points changed successfully!\n";
                        } else {
                            cout << "Failed to change points!\n";
                        }
                        cout << "Press ENTER to continue...\n";
                        cin.get();
                        break; // Thoát để quay lại menu cha
                    }
                    case 0: {
                        cout << "Returning to main menu...\n";
                        adminEditingMenuExit = true;
                        break;
                    }
                    default:
                        cout << "Invalid choice!\n";
                        cout << "Press ENTER key to continue...\n";
                        cin.get();
                        break;
                }
            } while(!adminEditingMenuExit);
            break;
        }
        default:
        break;
    }
    }
    }
   // Admin page menu
    else {
        cout  << "Login failed as admin" << endl;
        cout << "Press ENTER key to continue...\n";
        cin.get();
        return;
    }
    delete currentAdmin;
}

//Hàm thay đổi thông tin User có thể gọi được ở cả User và Admin
map<std::string, std::string> changeuserinfo (std::string& filename, 
                                                User *& currentUser, 
                                                bool isAdmin = false, 
                                                bool forceChangePassword = false, 
                                                bool isTargetAdmin = false) {
    
    if(!currentUser) {
        std::cerr << "Error: Current user is null!" << std::endl;
        return{};
    }
    map<string, string> updated_values; // Tạm lưu các thay đổi
    //cout << "DEBUG: Entering changeuserinfo function" << endl;
    while (true){
        int subChoice;
        if (forceChangePassword) {
            subChoice = 2; // Force change password
        } else {
        printchangeUserInfoMenu(isAdmin, isTargetAdmin);
        //cout << "Debug: Entering changeuserinfo function" << endl;
        //cin >> subChoice;
        //cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
        if (!(cin >> subChoice)) {
            cin.clear(); // Clear the error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid choice. Please try again." << endl;
            continue; // Skip to the next iteration of the loop
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore the newline character left in the input buffer
        }
        if(subChoice==1){ //đổi fullname user
            string newFullName;
            cout << "Enter new full Name (or 'z' to return to Menu): ";
            getline(cin, newFullName);
            newFullName = trim(newFullName);
            if (newFullName == "z" || newFullName == "Z") {
                cout << "Returning to menu..." << endl;
                continue; // Exit the loop to go back to the home menu
            }
            // Validate the new full name
            if(!isvalisfullName(newFullName)) {
                cout << "Invalid full name. Please try again." << endl;
                continue; // Skip to the next iteration of the loop
            }
            if(newFullName.empty()){
                cout << "Error: Full name cannot be empty!" << std::endl;
                continue;
            }
            
            //Xử lý OTP
            if(!isAdmin && !isTargetAdmin) {
                if(!verifyOTPForUser(currentUser)) {
                    cout << "OTP verification failed. Full name change cancelled!" << endl;
                    continue;
                }
            }
        
            currentUser->setFullName(newFullName); // Update the full name in the User object
            updated_values["Fullname"] = newFullName;
            cout << "Full name changed (pending save)!" << endl;
        //logTransaction(currentUser->wallet(), currentUser->accountName(), currentUser->fullName(), "", "", "", 0, true, "Full name changed successfully");
        } else if(subChoice==2){ //reset password user (set failedSatus = false, isGenaratedPass = true)
            string newPassword;
            if(isAdmin && !isTargetAdmin) {
                    // Admin reset password: sinh random
                    newPassword = generateSaltStr(12);
                    cout << "Generated new password for user: " << newPassword << endl;
                    //cout << "Please inform the user to login and change this password!" << endl;
            } else {
                while (true) {
                    cout << "Enter new password (or 'z' to return to Menu): ";
                    //cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    getline(cin, newPassword);
                    newPassword = trim(newPassword);
                    if (newPassword == "z" || newPassword == "Z") {
                    cout << "Returning to menu..." << endl;
                    return{}; // Exit the loop to go back to the home menu
                    }
                    // Validate the new password
                    if(!isvalidPassword(newPassword)) {
                    cout << "Invalid password. Please try again." << endl;
                    continue;
                    }
                    break;
                }
            }
            if(!newPassword.empty()){
                string salt = generateSaltStr();
                string hashedPassword = sha256(newPassword + salt);
                //Xử lý OTP
                if(!isAdmin && !isTargetAdmin) {
                if (!verifyOTPForUser(currentUser)) {
                    cout << "OTP verification failed. Password change cancelled!" << endl;
                    continue;
                }
            }
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            currentUser->setPassword(hashedPassword);
            updated_values["Password"] = hashedPassword;        
            updated_values["Salt"] = salt;
            cout << "Password changed (pending save)!" << endl;
            //while(cin.get() != '\n');

            if(isAdmin && !isTargetAdmin) {
                std::string statusFile = "../assets/userstatus.parquet";
                std::map<std::string, std::string> status_update;
                status_update["isGeneratedPassword"] = "true";
                status_update["failedLogin"] = "false";
                updateUserStatusRow(statusFile, currentUser->accountName(), status_update);
                cout << "User will be forced to change password at next login!" << endl;
            }
               // Nếu là đổi pass bắt buộc do đăng nhập bằng pass Random thì cập nhật isGeneratedPassword = "false"/"true"
            if (forceChangePassword) {
                std::string statusFile = "../assets/userstatus.parquet";
                std::map<std::string, std::string> status_update = {{"isGeneratedPassword", "false"}};
                updateUserStatusRow(statusFile, currentUser->accountName(), status_update);
                cout << "Press ENTER key to return Menu.....";
                //cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                while(cin.get() != '\n');
                break; // Thoát khỏi vòng lặp đổi mật khẩu bắt buộc
                }
            } 
            else{
                cout << "Password cannot be empty!" << endl;
                }
        } else if(subChoice == 3 && isAdmin) {
            string pointsInput;
            cout << "Enter points (The first add points for user max 100 point, or 'z' to return to Menu): " << endl;
            getline(cin, pointsInput);
            pointsInput = trim(pointsInput);
            if (pointsInput == "z" || pointsInput == "Z") {
                cout << "Returning to menu..." << endl;
                continue; // Exit the loop to go back to the home menu
            }
            // if (cin.fail()) {
            //     cin.clear(); // Clear the error state
            //     cin.ignore(numeric_limits<std::streamsize>::max(), '\n'); // Ignore invalid input
            //     cout << "Invalid point. Press ENTER key to return Menu....." << endl;
            //     cin.get();
            //     pointsInput = ""; // Exit if point is invalid
            // }
            int64_t editpoints = 0;
            try {
                editpoints = stoll(pointsInput);
            } catch (...) {
                cout << "Invalid point. Setting to 0!" << endl;
                editpoints = 0;
            }
            
            int64_t currentPoints = currentUser->point();
            int64_t newpoints = currentPoints + editpoints;
            //currentUser->setPoint(newpoints); // Update the points in the User object
            updated_values["Points"] = to_string(newpoints);
            cout << "Point changed (pending save)!" << endl;
            cout << "Press ENTER key to return Menu.....";
            cin.get();
            //logTransaction(currentUser->wallet(), currentUser->accountName(), currentUser->fullName(), "", "", "", 0, true, "Points changed successfully");
            //break; // Exit the loop to go back to the home menu
        } else if(subChoice==0){
            cout << "Back to main menu" << endl;
            //cout << "DEBUG: End of changeuserinfo" << endl;
            break; // Exit the loop to go back to the main menu
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
        //break;
    }
    return updated_values;
}

//Hàm kiểm tra năm nhuận
bool isLeapYear(int year) {
    // Check if the year is a leap year
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

//Hàm kiểm tra định dạng ngày (YYY-MM-DD HH:MM:SS)
bool isValidDateTime(const std::string& dateTime) {
    std::regex dateTimePattern(R"(\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2})");
    return std::regex_match(dateTime, dateTimePattern);
}

//Hàm chuyển định dạng ngày tháng năm thành DD-MM-YYYY
std::string convertDateTime(const std::string& dateTime, bool isEndOfDay = false) {
    if(dateTime.empty()) return "";
    std::regex dateTimePattern(R"(\d{2}/\d{2}/\d{4})");
    if(!std::regex_match(dateTime, dateTimePattern)) {
        std::cerr << "Invalid date format. Expected format: DD/MM/YYYY." << std::endl;
        return "";
    }
    int day, month, year;
    std::stringstream ss(dateTime);
    char delimiter1, delimiter2; // To handle the '/' delimiter
    if(!(ss >> day >> delimiter1 >> month >> delimiter2 >> year) || delimiter1 != '/' || delimiter2 != '/') {
        std::cerr << "Invalid date format. Expected format: DD/MM/YYYY." << std::endl;
        return "";
    }
    
    // std::cout << day << ", Month: " << month << ", Year: " << year << std::endl;
    // Extract day, month, year
    if(year < 1900 || year > 2100) {
        std::cerr << "Year out of range. Please enter a year between 1900 and 2100." << std::endl;
        return "";
    }
    if(month < 1 || month > 12) {
        std::cerr << "Invalid month. Please enter a month between 1 and 12." << std::endl;
        return "";
    }

    int daysInMonth [] = {31, (isLeapYear(year) ? 29 : 28), 31, 30, 31, 30, 
                                31, 31, 30, 31, 30, 31}; // Days in each month
    if(day < 1 || day > daysInMonth[month - 1]) {
        std::cerr << "Invalid day for the given month. Please enter a valid day." << std::endl;
        return "";
    }
    // Convert to YYYY-MM-DD format
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %s", year, month, day, isEndOfDay ? "23:59:59" : "00:00:00");
    return std::string(buffer);
}

//Hàm so sánh thời gian
bool isWithinTimeRange(const std::string& dateTime, 
                        const std::string& startDate, 
                        const std::string& endDate) {
    if(startDate.empty() && endDate.empty()) return true; // If both start and end dates are empty, return true
    if(!isValidDateTime(dateTime)) return false; // If dateTime is not valid, return false

    std::string adjustedStartDate = convertDateTime(startDate, false);
    std::string adjustedEndDate = convertDateTime(endDate, true);

    if(!startDate.empty() && adjustedStartDate.empty()) {
        return false; // Invalid start date format
    }
    if(!endDate.empty() && adjustedEndDate.empty()) {
        return false; // Invalid end date format
    }
    
    if(startDate.empty()) return dateTime <= adjustedEndDate; // If start date is empty, check if dateTime is before or equal to endDate
    if(endDate.empty()) return dateTime >= adjustedStartDate; // If end date is empty, check if dateTime is after or equal to startDate
    return dateTime >= adjustedStartDate && dateTime <= adjustedEndDate; // Check if dateTime is within the range
}

//truy vấn giao dịch từ logstransaction
void listTransactions(User* currentUser,
                     const string& username,
                     const string& IDWallet,
                     const string& startDate,
                     const string& endDate,
                     bool isAdmin,
                     const string& transactionType) {
    const string logFileName = "../logs/transaction.log";
    if (!filesystem::exists(logFileName)) {
        cout << "No transaction history found." << std::endl;
        return;
    }
    ifstream logFile(logFileName);
    if (!logFile.is_open()) {
        cerr << "Error opening transaction log file." << std::endl;
        return;
    }

    // Regex cho hai loại giao dịch
    regex transferPattern(R"(\[([^\s]+)\s+(\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2})\]\s+Transfer\s+From\s+WalletId\s*=\s*([^\s]+)\s+\(([^,]+),\s*([^)]+)\)\s+To\s+WalletId\s*=\s*([^\s]+)\s+\(([^,]+),\s*([^)]+)\)\s+Points transferred:\s*(\d+)\s+Status:\s*(\w+)(?:\s+Error=([^\n]*))?)");
    //regex pointEditPattern(R"(\[([^\s]+)\s+(\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2})\]\s*Point\s*Edit\s*By\s*WalletId\s*=\s*([^\s]+)\s*\(\s*([^,]+)\s*,\s*([^)]+)\s*\)\s*For\s*WalletId\s*=\s*([^\s]+)\s*\(\s*([^,]+)\s*,\s*([^)]+)\s*\)\s*Points\s*Change\s*[:\s]*(-?\d+)\s*\(\s*Reason\s*:\s*(.*?)\s*\)\s*Status\s*:\s*(\w+)(?:\s*Error\s*=\s*(.*?)\s*)?)");    
    
    bool allTransactions = isAdmin && username == "All" && IDWallet.empty();
    bool filterbyUsername = isAdmin && !username.empty() && username != "All";
    bool filterbyIDWallet = isAdmin && !IDWallet.empty() || (currentUser && IDWallet == currentUser->wallet());
    bool filterbyuser = !isAdmin && currentUser != nullptr;

    if (!allTransactions && !filterbyUsername && !filterbyIDWallet && !filterbyuser) {
        cout << "Invalid parameters: Provide currentUser or IDWallet for user mode, or username and IDWallet for admin mode." << std::endl;
        logFile.close();
        return;
    }

    if (!startDate.empty() && convertDateTime(startDate, false).empty()) {
        cerr << "Invalid start date format. Please use DD/MM/YYYY!" << std::endl;
        logFile.close();
        return;
    }

    if (!endDate.empty() && convertDateTime(endDate, true).empty()) {
        cerr << "Invalid end date format. Please use DD/MM/YYYY!" << std::endl;
        logFile.close();
        return;
    }

    string line;
    bool found = false;
    cout << "\n---- TRANSACTION HISTORY ----\n" << left << endl;
    cout /*<< std::setw(20) << "Transaction ID"*/
              << setw(20) << "DateTime"
              << setw(65) << "Sender Wallet ID"
              << setw(27) << "Sender Name"
              << setw(65) << "Receiver Wallet ID"
              << setw(27) << "Receiver Name"
              << setw(15) << "Points"
              << setw(10) << "Status"
              << setw(30) << "Error Message"
              << endl;
    cout << string(260, '-') << endl << flush;

    while (getline(logFile, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        smatch match;
        //bool isPointEdit = regex_match(line, match, pointEditPattern);
        bool isTransfer = regex_match(line, match, transferPattern);

        // if ((transactionType == "All" || (transactionType == "Transfer" && isTransfer) || (transactionType == "PointEdit" && isPointEdit)) && (isPointEdit || isTransfer)) {
        //     // std::cout << "DEBUG: Processing line: " << line << std::endl;
        //     // std::cout << "DEBUG: Match size: " << match.size() << std::endl;
        //     for (size_t i = 0; i < match.size(); ++i) {
        //         //std::cout << "DEBUG: match[" << i << "]: '" << match[i].str() << "'" << std::endl;
        //     }

            string dateTime = match[2].str();
            string senderWalletId = match[3].str();
            string senderFullName = match[5].str();
            string receiverWalletId = match[6].str();
            string receiverFullName = match[8].str();
            string pointsStr = match[9].str();
            string status = match[10].str();
            string errorMessage = match.size() > 12 ? match[12].str() : "";

            if (!isWithinTimeRange(dateTime, startDate, endDate)) {
                continue;
            }

            int64_t points = 0;
            try {
                points = stoll(pointsStr); // Thử chuyển đổi sang số
                //std::cout << "DEBUG: Successfully parsed points: " << pointsStr << " -> " << points << std::endl;
            } catch (const invalid_argument& e) {
                //std::cerr << "Invalid points value in log line: '" << line << "' - Error: " << e.what() << std::endl;
                continue; // Bỏ qua dòng log lỗi
            } catch (const out_of_range& e) {
                //std::cerr << "Points value out of range in log line: '" << line << "' - Error: " << e.what() << std::endl;
                continue; // Bỏ qua dòng log lỗi
            }

            bool matchCondition = false;
            if (allTransactions) {
                matchCondition = true;
            } else if (filterbyuser && currentUser) {
                matchCondition = (senderWalletId == currentUser->wallet() || receiverWalletId == currentUser->wallet());
            } else if (filterbyUsername) {
                matchCondition = (senderFullName == username || receiverFullName == username); // Sử dụng Fullname
            } else if (filterbyIDWallet) {
                matchCondition = (senderWalletId == IDWallet || receiverWalletId == IDWallet);
            } else if (allTransactions && username.empty()) {
                matchCondition = true; // Hiển thị tất cả nếu username rỗng và isAdmin
            }

            if (matchCondition) {
                found = true;
                cout << setw(20) << dateTime
                    << setw(65) << senderWalletId
                    << setw(27) << senderFullName
                    << setw(65) << receiverWalletId
                    << setw(27) << receiverFullName
                    << setw(15) << points
                    << setw(10) << status
                    << setw(30) << (errorMessage.empty() ? "-" : errorMessage)
                    << endl;
            }
    }
    logFile.close();
    if (!found) {
        cout << "No transactions found matching the criteria." << std::endl;
    } else {
        cout << "\nEnd of transaction history." << std::endl;
    }
}

//hàm liệt kê các giao dịch sửa điểm của admin
void listPointEdits(User* currentUser,
                    const string& username,
                    const string& IDWallet,
                    const string& startDate,
                    const string& endDate,
                    bool isAdmin,
                    const string& transactionType) {
    const string logFileName = "../logs/transaction.log";
    if (!filesystem::exists(logFileName)) {
        cout << "No transaction history found." << endl;
        return;
    }
    ifstream logFile(logFileName);
    if (!logFile.is_open()) {
        cerr << "Error opening transaction log file." << endl;
        return;
    }

    regex pointEditPattern(R"(\[([^\s]+)\s+(\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2})\]\s*Point\s*Edit\s*By\s*WalletId\s*=\s*([^\s]+)\s*\(\s*([^,]+)\s*,\s*([^)]+)\s*\)\s*For\s*WalletId\s*=\s*([^\s]+)\s*\(\s*([^,]+)\s*,\s*([^)]+)\s*\)\s*Points\s*Change\s*[:\s]*(-?\d+)\s*\(\s*Reason\s*:\s*(.*?)\s*\)\s*Status\s*:\s*(\w+)(?:\s*Error\s*=\s*(.*?)\s*)?)");
    string line;
    bool found = false;
    cout << "\n---- EDITS POINT HISTORY ----\n" << left << endl;
    cout /*<< setw(20) << "Transaction ID"*/
         << setw(20) << "DateTime"
         << setw(18) << "Editor Wallet ID"
         << setw(13) << "Editor Name"
         << setw(65) << "Target Wallet ID"
         << setw(27) << "Target Name"
         << setw(15) << "Points"
         << setw(45) << "Reason"
         << setw(10) << "Status"
         << setw(30) << "Error Message" << endl;
    cout << string(260, '-') << endl << flush;

    // Điều kiện lọc
    bool allTransactions = isAdmin && username == "All" && IDWallet.empty();
    bool filterbyUsername = isAdmin && !username.empty() && username != "All";
    bool filterbyIDWallet = isAdmin && !IDWallet.empty() || (!isAdmin && currentUser && IDWallet == currentUser->wallet());
    bool filterbyuser = !isAdmin && currentUser != nullptr;

    if (!allTransactions && !filterbyUsername && !filterbyIDWallet && !filterbyuser) {
        cout << "Invalid parameters: Provide currentUser or IDWallet for user mode, or username and IDWallet for admin mode." << endl;
        logFile.close();
        return;
    }

    while (getline(logFile, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        smatch match;
        if (regex_match(line, match, pointEditPattern)) {
            // cout << "DEBUG: Processing line: " << line << endl;
            // cout << "DEBUG: Match size: " << match.size() << endl;
            for (size_t i = 0; i < match.size(); ++i) {
                //cout << "DEBUG: match[" << i << "]: '" << match[i].str() << "'" << endl;
            }

            string dateTime = match[2].str();
            string editorWalletId = match[3].str();
            string editorFullName = match[5].str();
            string targetWalletId = match[6].str();
            string targetFullName = match[8].str();
            string pointsStr = match[9].str();
            string reason = match[10].str();
            string status = match[11].str();
            string errorMessage = match.size() > 12 ? match[12].str() : "";

            if (!isWithinTimeRange(dateTime, startDate, endDate)) continue;

            int64_t points = 0;
            if (!pointsStr.empty()) {
                try {
                    points = stoll(pointsStr);
                    //cout << "DEBUG: Successfully parsed points: " << pointsStr << " -> " << points << endl;
                } catch (const exception& e) {
                    cerr << "Invalid points value in log line: '" << line << "' - Error: " << e.what() << endl;
                    continue;
                }
            }

            bool matchCondition = false;
            if (allTransactions) {
                matchCondition = true;
            } else if (filterbyuser && currentUser) {
                matchCondition = (editorWalletId == currentUser->wallet() || targetWalletId == currentUser->wallet());
            } else if (filterbyUsername) {
                matchCondition = (editorFullName == username || targetFullName == username);
            } else if (filterbyIDWallet) {
                matchCondition = (editorWalletId == IDWallet || targetWalletId == IDWallet);
            }

            if (matchCondition) {
                found = true;
                cout /*<< setw(20) << match[1].str() // Transaction ID*/
                     << setw(20) << dateTime
                     << setw(18) << editorWalletId
                     << setw(13) << editorFullName
                     << setw(65) << targetWalletId
                     << setw(27) << targetFullName
                     << setw(15) << points
                     << setw(45) << reason
                     << setw(10) << status
                     << setw(30) << (errorMessage.empty() ? "-" : errorMessage)
                     << endl;
            }
        }
    }

    logFile.close();
    if (!found) cout << "No transactions found matching the criteria." << endl;
    else cout << "\nEnd of transaction history." << endl;
}

void eWallet(User *& currentUser) {
    while (true){
        int subChoice;
        cout << "\nUser: " << currentUser->accountName() << endl;
        cout << "\n--- E-Wallet ---\n" << "Balance: " << currentUser -> point() << " points" << endl;
        cout << "--------------------\n" << endl;
        printeWalletMenu();
        cin >> subChoice;
        cin.ignore(); // Ignore the newline character left in the input buffer
        if(subChoice==1){
            std::string filename = "../assets/users.parquet";
            try {
            arrow::Status status = transferPoint(filename, currentUser);
            if (!status.ok()) {
                cout << "Error transferring points: " << status.ToString() << endl;
                continue;
            } else {
                cout << "Points transferred successfully!" << endl;
            }
        }
        catch (const std::exception& e) {
            cout << "Exception occurred: " << e.what() << endl;
        } catch (...) {
            cout << "An unknown error occurred." << endl;
        }
        cout << "Press ENTER key to return Menu.....";
        cin.get();
        } else if(subChoice==2){
            cout << "Are you sure you want to see All Transaction History? (Y/N) (or 'z' to return to E-Wallet Menu): ";
            string confirm;
            getline(cin, confirm);
            if (confirm == "Z" || confirm == "z") {
                cout << "Returning to E-Wallet Menu..." << endl;
                continue; // Return to the eWallet menu
            } else if (confirm != "Y" && confirm != "y") {
                cout << "Transaction history viewing cancelled." << endl;
                continue; // Cancel viewing transaction history
            }
            cout << "\nTransaction all history for user: " << currentUser->accountName() << endl;
            cout << "---------------------------------" << endl;
            listTransactions(currentUser, "", "", "", "", false, "Transfer"); // Call the function to list transactions for the current user
            cout << "Returning to E-Wallet Menu..." << endl;
            continue; // Return to the eWallet menu
        } else if(subChoice==3){
            string startDate, endDate;
            cout << "Input startdate (DD/MM/YYYY) (or 'z' to return menu):" << endl;
            getline(cin, startDate);
            if (startDate == "z" || startDate == "Z") {
                std::cout << "Returning to menu..." << std::endl;
                break; // Exit the loop to go back to the menu
            }
            cout << "Input enddate (DD/MM/YYYY) (or 'z' to return menu):" << endl;
            getline(cin, endDate);
            if (endDate == "z" || endDate == "Z") {
                std::cout << "Returning to menu..." << std::endl;
                break; // Exit the loop to go back to the menu
            }
            cout << "\nTransaction History by Time: from " << startDate << " to " << endDate << endl;
            cout << "--------------------" << endl;
            listTransactions(currentUser, "", "", startDate, endDate, false, "Transfer");
            cout << "Returning to E-Wallet Menu..." << endl;
            continue; // Return to the eWallet menu
        } else if(subChoice==0){
            cout << "Back to main menu" << endl;
            break; // Exit the loop to go back to the main menu
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

// Hàm đăng nhập vào User Home Menu
void UserLoginMenu(User *& currentUser) {
    // if (!currentUser->check_consumer_thread_running()) {
    //     currentUser->activateConsumerThread();
    // }
    while (true){
        int choice;
        cout << "\nUser: " << currentUser->accountName() << endl;
        printUserLoginMenu();
        if(!(cin >> choice)) {
            cin.clear(); // Clear the error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid choice. Please try again." << endl;
            continue; // Skip to the next iteration of the loop
        }
        
        if(choice == 1){ // Thay đổi thông tin tài khoản
            printUserInfoFromDb(currentUser);
            string filename = "../assets/users.parquet"; 
            //cout << "DEBUG: End of changeuserinfo" << endl;
            auto updated_value = changeuserinfo(filename, currentUser, false, false, false); // Call the function to change user info
            //cout << "DEBUG: End of changeuserinfo" << endl
            if(!updated_value.empty()) {
                arrow::Status status = updateUserInfo(filename, currentUser, updated_value);
                if(!status.ok()){
                    cout << "Error updating user info: " << status.ToString() << endl;
                } else {
                    cout << "All changes saved successfully!" << endl;
                }
            }
        } else if(choice == 2){ // gọi menu ewallet
            eWallet(currentUser);
        } else if(choice == 3){ //xóa tài khoản
            string confirm;
            cout << "Are you sure to delete your account? (Y/N):";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, confirm);
            confirm = trim(confirm);
            if(confirm == "Y" || confirm == "y") {
            
                //Xử lý OTP
                int otpAttempts = 0;
                const int maxOtpAttempts = 3;
                bool otpVerified = false;
                std::string otp, userOtp;

                while (otpAttempts < maxOtpAttempts) {
                otp = generateOTP(currentUser->wallet(), currentUser->accountName());
                std::cout << "Your OTP is: " << otp << std::endl;
                std::cout << "Enter the OTP: ";
                getline(std::cin, userOtp);
                userOtp = trim(userOtp);
                if (userOtp == otp) {
                    otpVerified = true;
                    break;
                } else {
                    std::cout << "Invalid OTP. Please try again." << std::endl;
                    otpAttempts++;
                    }
                }

                if(!verifyOTP(userOtp, currentUser->wallet(), currentUser->accountName())) {
                    std::cout << "You entered incorrect OTP 3 time. Delele account cancelled." << std::endl;
                    cout << "Press ENTER key to return Menu.....";
                    cin.get();
                    return;
                }
            string fileStatus = "../assets/userstatus.parquet";
            map<string, string> update_values = {{"deleteUser", "true"}};
            if(updateUserStatusRow(fileStatus, currentUser->accountName(), update_values)) {
                cout << "Account was deleted!" << endl;
            } else {
                cout << "Account was not deleted, Please contact Admin!" << endl;

            }
            cout << "Press ENTER key to return Menu.....";
            cin.get();
            break; // Exit the loop to log out
            } 
        } else if(choice == 0){
            cout << "Logging out..." << endl;
            break; // Exit the loop to log out
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

void userHomeMenu(Client *& currentClient) {
    while (true){
        system("cls");
        int choice;
        printuserHomeMenu();
        if(!(cin >> choice)) {
            cin.clear(); // Clear the error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid choice. Please try again." << endl;
            continue; // Skip to the next iteration of the loop
        }
        
        shared_ptr<arrow::io::ReadableFile> infile; //

        if(choice == 1){
            User * currentUser = nullptr;
            loginUser(infile, currentUser);
            
            
        } else if(choice == 2){
            // User * 
            currentClient = enterUserInfoRegister(false, false);
            User * currentUser = dynamic_cast<User*>(currentClient);
            if (currentClient == nullptr) {
                continue; // Return to the user home menu
            }
            std::string filename = "../assets/users.parquet";
            std::string fullName = currentUser->fullName();
            std::string accountName = currentUser->accountName();  
            std::string password = currentUser->password();
            std::string salt = currentUser->salt();
            int64_t point = currentUser->point();
            std::string wallet = currentUser->wallet();
            arrow::Status status = AppendUserParquetRow(filename, fullName, accountName, password, salt, point, wallet);
            cout << "DEBUG: Status after AppendUserParquetRow: " << status.ToString() << endl;
            if(!status.ok()) {
                cout << "Error registering user: " << status.ToString() << endl;
                return;
            }
            UserLoginMenu(currentUser);
            delete currentUser; // Clean up the dynamically allocated user object
        } else if(choice == 0){
            cout << "Back to Main Menu..." << endl;
            break; // Exit the loop to go back to the main menu
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

void mainMenu(Client *& currentClient) {
    while (true){
        system("cls");
        int choice;
        printmainMenu();
        if(!(cin >> choice)) {
            cin.clear(); // Clear the error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid choice. Please try again." << endl;
            continue; // Skip to the next iteration of the loop
        }
        cin.ignore(); // Ignore the newline character left in the input buffer
        
        if(choice == 1){
            userHomeMenu(currentClient);
        } else if(choice == 2){
            AdminLoginMenu(currentClient);
        } else if(choice == 0){
            cout << "Goodbye!..." << endl;
            exit(0); // Exit the loop to log out
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}