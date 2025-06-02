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


void printAdminHomeMenu() {
    cout << "--- ADMIN HOME MENU ---" << endl;
    cout << "---------------------------------" << endl;
    cout << "1. User Listing" << endl;
    cout << "2. Manage User" << endl;
    cout << "3. Wallet Management" << endl;
    cout << "4. Admin Info Editing" << endl;
    cout << "0. Back" << endl;
    cout << "---------------------------------" << endl;
    cout << "Enter Your Option: ";
}

void printAdminWalletMenu() {
    cout << "--- ADMIN WALLET MANAGE MENU ---" << endl;
    cout << "---------------------------------" << endl;
    cout << "1. Check Balance" << endl;
    cout << "2. History add point" << endl;
    cout << "3. History transaction" << endl;
    cout << "0. Back" << endl;
    cout << "---------------------------------" << endl;
    cout << "Enter Your Option: ";
}

void printAdminEditMenu() {
    cout << "--- ADMIN INFO EDITING MENU ---" << endl;
    cout << "---------------------------------" << endl;
    cout << "1. Change name" << endl;
    cout << "2. Change password" << endl;
    cout << "0. Back" << endl;
    cout << "---------------------------------" << endl;
    cout << "Enter Your Option: ";
}

void printUserEditMenu() {
    cout << "--- ADMIN MANAGE USER MENU ---" << endl;
    cout << "---------------------------------" << endl;
    cout << "1. Add User" << endl;
    cout << "2. Add Users From CSV" << endl;
    cout << "3. Edit User Info" << endl;
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
void printchangeUserInfoMenu(bool isAdmin) {
    //try{
    //cout << "Debudging change user info menu" << endl;
    cout << "\n--- CHANGE USER INFO MENU ---\n";
    cout << "---------------------------------\n";
    cout << "1. Change Full Name\n"; 
    cout << "2. Change Password\n";
    if (isAdmin) { cout << "3. Edit Point\n"; }
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
        cout << "Password is too short. Minimum length is 8 characters." << endl;
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

    return hasUpper && hasLower && hasDigit && hasSpecial;
}

User * enterUserInfo(){
    system("cls");
    string fullName;
    string userName;
    string password;
    int point;

    cout << "ENTER USER INFO" << endl;
    cout << "---------------------------" <<endl;
    cout << "User fullname: ";
    //cin.ignore();
    getline(cin, fullName);
    cout << "User username: ";
    getline (cin, userName);
    cout << "User password: ";
    getline (cin, password);
    cout << "User user-point: ";
    cin >> point;


    User * user = new User();
    return user;    
}

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
    do {
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
    }
    } while (!isvalisfullName(FullName));

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

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the newline character left in the input buffer
    
    bool isGeneratedPassword = false; // Flag to indicate if the password is generated
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
    } else {
        cout << "User password (leave empty to generate random password, or 'z' to return to menu): ";
        getline(cin, Password);
        Password = trim(Password);
        if (Password == "z" || Password == "Z") {
            cout << "User creation cancelled." << endl;
            cout << "Return to menu..." << endl;
            return nullptr; // Return to menu if user enters 'z'
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
return user;
}

// Edit user of Admin
bool UserEditMenu(Admin * currentAdmin) {
    int userEditMenuOption;
    do {
        system("cls");
        printUserEditMenu();
        cin >> userEditMenuOption;

    } while (userEditMenuOption < 0 || userEditMenuOption>4);
    switch (userEditMenuOption)
    {
        case 0:
            return true;
        case 1: {
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
            int point = user->point();
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

        case 2: {
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

        case 3: {
                string filename = "../assets/users.parquet";
                string userName;
                cout << "Enter user name for edit (or 'z' to return Menu): ";
                cin.ignore(numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
                getline (cin, userName);
                userName = trim(userName);
                if (userName == "z" || userName == "Z") {
                    std::cout << "Returning to Menu..." << std::endl;
                    string currentUser = nullptr;
                    break;
                }
                std::shared_ptr<arrow::io::ReadableFile> infile;
                try {
                    PARQUET_ASSIGN_OR_THROW(infile, arrow::io::ReadableFile::Open(filename));
                } catch (const arrow::Status& status) {
                    std::cerr << "Error opening file: " << status.ToString() << std::endl;
                    cout << "Returning to Menu..." << endl;
                    std::cin.get(); // Wait for user input before continuing
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
                        cout << "User edit: " << currentUser->accountName() << endl;
                        break; 
                    }
                }
                if (!userfound) {
                    std::cout << "User " << userName <<" not found! (User invalid)" << std::endl;
                    cout << "Please check the user name and try again." << endl;
                    cin.get(); // Wait for user input before continuing
                    break;
                }
                if (!currentUser) {
                    cout << "User: " << currentUser << " not found!" << endl;
                    cout << "Please check the user name and try again." << endl;
                    cin.get();
                    break; // Skip to the next iteration of the loop
                }
                                
                map<string, string> pending_updates; // Create a map to hold pending updates
                
                changeuserinfo(filename, currentUser, pending_updates, true); // Call the function to change user info
                if (!pending_updates.empty()) {
                arrow::Status status = updateUserInfo(filename, currentUser, pending_updates); // Update the user info in the database
                if (!status.ok()) {
                    cout << "Error updating user info: " << status.ToString() << endl;
                    //logTransaction(currentUser->wallet(), currentUser->accountName(), currentUser->fullName(), "", "", "", 0, false, "Failed to update user info");
                    cin.get(); // Wait for user input before continuing
                    return false; // Exit if the update fails
                }
                cout << "User info updated successfully!" << endl;
                //logTransaction(currentUser->wallet(), currentUser->accountName(), currentUser->fullName(), "", "", "", 0, true, "User info updated successfully");
                }
                //cout << "DEBUG: Returned from changeuserinfo" << endl;
                cout << "User info changed successfully!" << endl;
                cin.get(); // Wait for user input before continuing
            break;
        }
        default:
            break;
    }

    return false;
}

bool loginAdmin(Admin *& currentAdmin) {
    string adminName;
    string adminPassword;
    // string userName;
    cout << "User admin: ";
    cin >> adminName;
    cout << "Password admin: ";
    cin >> adminPassword;

    if (adminName == "admin" && adminPassword == "admin") {
        // currentAdmin->setAccountName(adminName);
        // currentAdmin->setPassword(adminPassword);
        return true;
    } else {
        return false;
    }
    // return false;
}

void AdminLoginMenu(Client *& currentClient) {
    currentClient = new Admin();
    Admin * currentAdmin = dynamic_cast<Admin*>(currentClient);

    // shared_ptr<arrow::io::ReadableFile> infile;
      
    if (loginAdmin(currentAdmin)) {
        bool adminLoginMenuExit = false;
        while (!adminLoginMenuExit)
        {
            /* code */
            system("cls");
            int adminHomeMenuOption;
            do {
                system("cls");
                printAdminHomeMenu();
                cin >> adminHomeMenuOption;
                cout << "Lua chon cua ban la" << adminHomeMenuOption << endl;
    
            } while (adminHomeMenuOption < 0 || adminHomeMenuOption>4);
    
            switch (adminHomeMenuOption)
            {
            case 0:
                adminLoginMenuExit = true; 
                break;
            case 1: {
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
            case 2: {
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
            case 3: { //Wallet Management
                system("cls");
                
                bool exit = false;
                do { 
                    
                    printAdminWalletMenu();
                 cout << "Admin management" << endl; 
                /* gọi hàm tính tổng ví của các user trong file user.parquet và so sanh với piont của admin trong file admin.parquet */
                            
                    std::cout << "Enter Z to go back: ";
                    std::string exit_char;
                    std::cin >> exit_char;
                    if (exit_char == "Z" || exit_char == "z") {
                        exit = true;
                    }
                    /* code */
                } while (!exit);             
                break;
            }

            case 4:{
                system("cls");
                
                bool exit = false;
                do { 
                    
                    printAdminEditMenu();
                 cout << "Admin Info Editing" << endl; 
                                            
                    std::cout << "Enter Z to go back: ";
                    std::string exit_char;
                    std::cin >> exit_char;
                    if (exit_char == "Z" || exit_char == "z") {
                        exit = true;
                    }
                    /* code */
                } while (!exit);             
                break;
            }
            
            default:
                break;
            }
            
        }

        // Admin page menu
    } else {
        cout  << "Login failed as admin" << endl;

    }
    delete currentAdmin;

}

/// @brief 
/// @param filename 
/// @param currentUser 
/// @param isAdmin 
/// @return 
void changeuserinfo(std::string& filename, User *& currentUser, std::map<std::string, std::string>& pending_updates, bool isAdmin) {
    
    if(!currentUser) {
        cout << "Error: Current user is null!" << endl;
        return; // Exit if currentUser is null
    }
  
    //cout << "DEBUG: Entering changeuserinfo function" << endl;
    while (true){
        printchangeUserInfoMenu(isAdmin);
        //cout << "Debug: Entering changeuserinfo function" << endl;
        int subChoice;
        //cin >> subChoice;
        //cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
        if (!(cin >> subChoice)) {
            cin.clear(); // Clear the error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid choice. Please try again." << endl;
            continue; // Skip to the next iteration of the loop
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore the newline character left in the input buffer
        if(subChoice==1){
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
            if(!isAdmin) {
                if(!verifyOTPForUser(currentUser)) {
                cout << "OTP verification failed. User info change cancelled!" << std::endl;
                logTransaction(currentUser->wallet(), currentUser->accountName(), currentUser->fullName(), "", "", "", 0, false, "OTP verification failed");
                continue; // Exit if OTP verification fails
                }
            }
        
        pending_updates["Fullname"] = newFullName; // Add the updated full name to the pending updates map
        currentUser->setFullName(newFullName); // Update the full name in the User object
        // map<string, string> updated_values = {
        //     {"FullName", newFullName} // Prepare the updated values for the database
        // };

        // arrow::Status status = updateUserInfo(filename, currentUser, updated_values); // Update the user info in the database
        //     if (!status.ok()) {
        //         cout << "Error updating user info: " << status.ToString() << endl;
        //         logTransaction(currentUser->wallet(), currentUser->accountName(), currentUser->fullName(), "", "", "", 0, false, "Failed to update user Full Name");
        //         continue;
        //     }
        // cout << "Full name changed successfully!" << endl;
        // logTransaction(currentUser->wallet(), currentUser->accountName(), currentUser->fullName(), "", "", "", 0, true, "Full name changed successfully");
        } else if(subChoice==2){
            string newPassword;
            cout << "Enter new password (or 'z' to return to Menu): ";
            getline(cin, newPassword);
            newPassword = trim(newPassword);
            if (newPassword == "z" || newPassword == "Z") {
                cout << "Returning to menu..." << endl;
                continue; // Exit the loop to go back to the home menu
            }
            // Validate the new password
            if(!isvalidPassword(newPassword)) {
                cout << "Invalid password. Please try again." << endl;
                continue; // Skip to the next iteration of the loop
            }
            cout << "New password entered: " << newPassword << endl;
            
            if(!isAdmin) {
                if(!verifyOTPForUser(currentUser)) {
                    cout << "OTP verification failed. User info change cancelled!" << std::endl;
                    logTransaction(currentUser->wallet(), currentUser->accountName(), currentUser->fullName(), "", "", "", 0, false, "OTP verification failed");
                    continue; // Exit if OTP verification fails
                }
            }

            string salt = currentUser->salt();
            string hashedPassword = sha256(newPassword + salt);
            pending_updates["Salt"] = salt; // Add the salt to the pending updates map
            pending_updates["Password"] = hashedPassword; // Add the updated password to the pending updates map
            currentUser->setPassword(hashedPassword);

        } else if(subChoice == 3 && isAdmin) {
            int newpoints;
            cout << "Enter points (The first add points max 100 point): " << endl;
            cin >> newpoints;
            if (cin.fail() || newpoints < 0) {
                cin.clear(); // Clear the error state
                cin.ignore(numeric_limits<std::streamsize>::max(), '\n'); // Ignore invalid input
                cout << "Invalid point. Setting to 0" << endl;
                newpoints = 0; // Exit if point is invalid
            }
            pending_updates["Points"] = newpoints; // Add the updated points to the pending updates map
            currentUser->setPoint(newpoints); // Update the points in the User object
        }
        
        else if(subChoice==0){
            cout << "Back to main menu" << endl;
            //cout << "DEBUG: End of changeuserinfo" << endl;
            return; // Exit the loop to go back to the home menu
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

bool isLeapYear(int year) {
    // Check if the year is a leap year
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

//Hàm kiểm tra định dạng ngày (YYY-MM-DD HH:MM:SS)
bool isValidDateTime(const std::string& dateTime) {
    std::regex dateTimePattern(R"(\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2})");
    return std::regex_match(dateTime, dateTimePattern);
}

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
void listTransactions(User* currentUser = nullptr,
                      const std::string& username = "",
                      const std::string& IDWallet = "",
                      const std::string& startDate = "",
                      const std::string& endDate = "",
                      bool isAdmin = false) {
    const std::string logFileName = "../logs/transaction.log";
    if (!std::filesystem::exists(logFileName)) {
        std::cout << "No transaction history found." << std::endl;
        return;
    }
    std::ifstream logFile(logFileName);
    if (!logFile.is_open()) {
        std::cerr << "Error opening transaction log file." << std::endl;
        return;
    }

    //Regex to match transaction lines
    std::regex logPattern(R"(\[([^\s]+)\s+(\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2})\]\s+Transfer\s+From\s+WalletId\s*=\s*([^\s]+)\s+\(([^,]+),\s*([^)]+)\)\s+To\s+WalletId\s*=\s*([^\s]+)\s+\(([^,]+),\s*([^)]+)\)\s+Points transferred:\s*(\d+)\s+Status:\s*(\w+)(?:\s+Error=([^\n]*))?)");

    // kiểm tra tham số
    bool allTransactions = isAdmin && username == "All" && IDWallet.empty(); // Hiển thị tất cả giao dịch nếu là admin và không lọc theo username hoặc IDWallet
    bool filterbyUsername = isAdmin && !username.empty() && username != "All"; // Lọc theo username nếu là admin và username không rỗng hoặc không phải "All"
    bool filterbyIDWallet = isAdmin && !IDWallet.empty() || (currentUser && IDWallet == currentUser->wallet()); // Lọc theo IDWallet nếu là admin và IDWallet không rỗng, hoặc nếu là người dùng hiện tại và IDWallet trùng với IDWallet của người dùng
    bool filterbyuser = !isAdmin && currentUser != nullptr; // Lọc theo người dùng nếu không phải là admin và currentUser không rỗng

    if (!allTransactions && !filterbyUsername && !filterbyIDWallet && ! filterbyuser) {
        cout <<"Invalid parameters: Provide currentUser or IDWallet for user mode, or username and IDWallet for admin mode." << std::endl;
        logFile.close();
        return;
    }
    
    if(!startDate.empty() && convertDateTime(startDate, false).empty()) {
        std::cerr << "Invalid start date format. Please use DD/MM/YYYY!" << std::endl;
        logFile.close();
        return;
    }

    if(!endDate.empty() && convertDateTime(endDate, true).empty()) {
        std::cerr << "Invalid end date format. Please use DD/MM/YYYY!" << std::endl;
        logFile.close();
        return;
    }

    std::string line;
    bool found = false;
    std::cout << std::left;
    std::cout /*<< std::setw(12) << "Transaction ID" */
              << std::setw(20) << "DateTime" 
              << std::setw(67) << "Sender Wallet ID" 
              << std::setw(20) << "Sender Full Name" 
              << std::setw(67) << "Receiver Wallet ID" 
              << std::setw(20) << "Receiver Full Name" 
              << std::setw(10) << "Points" 
              << std::setw(10) << "Status"
              << std::setw(50) << "Error Message" 
              << std::endl;
    std::cout << std::string(240, '-') << std::endl;
    
    while (std::getline(logFile, line)) {
        std::smatch match;
        if (regex_match(line, match, logPattern)) {
            std::string transactionId = match[1].str();
            std::string dateTime = match[2].str();
            std::string senderWalletId = match[3].str();
            std::string senderUserName = match[4].str();
            std::string senderFullName = match[5].str();
            std::string receiverWalletId = match[6].str();
            std::string receiverUserName = match[7].str();
            std::string receiverFullName = match[8].str();
            int64_t pointsTransferred = std::stoll(match[9].str());
            std::string status = match[10].str();
            std::string errorMessage = match.size() > 9 ? match[11].str() : "";

            // Kiểm tra điều kiện lọc
            if(!isWithinTimeRange(dateTime, startDate, endDate)) {
                continue; // Skip this transaction if it is not within the specified time range
            }
            bool matchCondition = false;
            if (allTransactions) { 
                matchCondition = true; // If all transactions are requested, match all conditions
            } else if (filterbyuser && currentUser) {
                matchCondition = (senderWalletId == currentUser->wallet() || receiverWalletId == currentUser->wallet());
            } else if (filterbyUsername) {
                // std::string senderUserName = match[4].str().substr(0, match[4].str().find(","));
                // std::string receiverUserName = match[6].str().substr(0, match[6].str().find(","));
                matchCondition = (senderUserName == username || receiverUserName == username);
            }
            else if (filterbyIDWallet) {
                matchCondition = (senderWalletId == IDWallet || receiverWalletId == IDWallet);
            }

            if(matchCondition) {
                found = true;
                std::string shorttransactionId = transactionId.length() > 10 ? transactionId.substr(0, 10) + "..." : transactionId; // Truncate transaction ID to 12 characters
                std::cout /*<< std::setw(20) << transactionId */
                          << std::setw(20) << dateTime 
                          << std::setw(67) << senderWalletId 
                          << std::setw(20) << senderFullName 
                          << std::setw(67) << receiverWalletId 
                          << std::setw(20) << receiverFullName 
                          << std::setw(10) << pointsTransferred 
                          << std::setw(10) << status
                          << std::setw(50) << errorMessage << std::endl;

            }
        }
    }
    logFile.close();
    if (!found) {
        std::cout << "No transactions found matching the criteria." << std::endl;
    } else {
        std::cout << "End of transaction history." << std::endl;
    }
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
            cout << "Transaction history for user: " << currentUser->accountName() << endl;
            cout << "---------------------------------" << endl;
            listTransactions(currentUser, "", "", "", "", false); // Call the function to list transactions for the current user
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
            cout << "Transaction History by Time:" << endl;
            cout << "--------------------" << endl;
            listTransactions(currentUser, "", "", startDate, endDate, false);
        } else if(subChoice==0){
            cout << "Back to main menu" << endl;
            break; // Exit the loop to go back to the main menu
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

void UserLoginMenu(User *& currentUser) {
    if (!currentUser->check_consumer_thread_running()) {
        currentUser->activateConsumerThread();
    }
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
        
        if(choice == 1){
           printUserInfoFromDb(currentUser);
           std::string filename = "../assets/users.parquet";
            map<string, string> pending_updates; // Create a map to hold pending updates
           //cout << "DEBUG: End of changeuserinfo" << endl;
           changeuserinfo(filename, currentUser, pending_updates, false); // Call the function to change user info
           if (!pending_updates.empty()) {
                arrow::Status status = updateUserInfo(filename, currentUser, pending_updates); // Update the user info in the database
                if (!status.ok()) {
                    cout << "Error updating user info: " << status.ToString() << endl;
                    logTransaction(currentUser->wallet(), currentUser->accountName(), currentUser->fullName(), "", "", "", 0, false, "Failed to update user info");
                    continue; // Skip to the next iteration of the loop
                }
                cout << "User info updated successfully!" << endl;
                //logTransaction(currentUser->wallet(), currentUser->accountName(), currentUser->fullName(), "", "", "", 0, true, "User info updated successfully");
            }
           //cout << "DEBUG: End of changeuserinfo" << endl;
        } else if(choice == 2){
            eWallet(currentUser);
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
            int point = currentUser->point();
            std::string wallet = currentUser->wallet();
            arrow::Status status = AppendUserParquetRow(filename, fullName, accountName, password, salt, point, wallet);
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