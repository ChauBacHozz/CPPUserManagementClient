#include "Menus.h"
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
using namespace std;


void printAdminHomeMenu() {
    cout << "ADMIN HOME MENU" << endl;
    cout << "---------------------------------" << endl;
    cout << "1. User listing" << endl;
    cout << "2. User edit" << endl;
    cout << "3. Wallet management" << endl;
    cout << "4. Admin info editing" << endl;
    cout << "0. Exit" << endl;
    cout << "---------------------------------" << endl;
    cout << "Enter your option: ";
}
void printUserEditMenu() {
    cout << "USER EDITING MENU" << endl;
    cout << "---------------------------------" << endl;
    cout << "1. Add user" << endl;
    cout << "2. Add users from csv" << endl;
    cout << "0. Exit" << endl;
    cout << "---------------------------------" << endl;
    cout << "Enter your option: ";
}
void printUserInfoFromDb(User *& currentUser) {
    cout << "\n--- User Info ---\n";
    cout << "Full Name: " << currentUser->fullName() << endl;
    cout << "User Name: " << currentUser->accountName() << endl;
}

void printchangeUserInfoMenu() {
    cout << "\n--- CHANGE USER INFO MENU ---\n" 
         << "1. Change Full Name\n" 
         << "2. Change Password\n" 
         << "3. Back\n";
    cout << "Enter your choice: ";
}

void printeWalletMenu() {
    cout << "\n--- E-WALLET MENU ---\n" 
         << "1. Transfer Points\n" 
         << "2. Transaction History\n" 
         << "3. Transaction History by Time\n" 
         << "4. Back\n";
    cout << "Enter your choice: ";
}

void printUserLoginMenu() {
    cout << "\n--- USER LOGIN MENU ---\n" 
         << "1. Information\n" 
         << "2. E-Walelt\n" 
         << "3. Back\n";
    cout << "Enter your choice: ";
}

void printuserHomeMenu() {
    cout << "\n--- USER HOME MENU ---\n" 
         << "1. Login\n" 
         << "2. Register\n" 
         << "3. Back\n";
    cout << "Enter your choice: ";
}

void printmainMenu() {
    cout << "\n--- MAIN MENU ---\n" 
         << "1. User Home\n" 
         << "2. Admin Home\n" 
         << "3. Exit\n";
    cout << "Enter your choice: ";
}

// std::string trim(const std::string& str) {
//     size_t first = str.find_first_not_of(' \t\n\r');
//     size_t last = str.find_last_not_of(' \t\n\r');
//     if (first == std::string::npos || last == std::string::npos) {
//         return ""; // No non-whitespace characters found
//     }
//     return str.substr(first, (last - first + 1));
// }

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
    cin.ignore();
    getline(cin, fullName);
    cout << "User username: ";
    cin >> userName;
    cout << "User password: ";
    cin >> password;
    cout << "User user-point: ";
    cin >> point;


    User * user = new User();
    return user;    
}

User * enterUserInfoRegister(bool enterPoint = false){
    system("cls");
    string FullName;
    string userName;
    string password;
    string genPassword;
    string genSalt;
    string genWalletId;
    cout << "ENTER USER INFO" << endl;
    cout << "---------------------------" <<endl;
    do {
    cout << "User FullName: ";
    cin.ignore();
    getline(cin, FullName);
    FullName = trim(FullName);
    if(!isvalisfullName(FullName)) {
        cout << "Invalid full name. Please try again." << endl;
    }
    } while (!isvalisfullName(FullName));

    while (true){
        cout << "User username: ";
        cin >> userName;
        if (isUserExist(userName)) {
            cout << "Username already exists. Please try again." << endl;
        } 
        else {
            cout << "Username is valid." << endl;
            break; // Exit the loop if the username is unique
        }  
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
    
    do {
        cout << "User password: "; 
        cin.ignore();
        getline(cin, password);
        password = trim(password);
        genSalt = generateSaltStr();

        if (!password.empty()) {
            if (!isvalidPassword(password)) {
                std::cout << "Invalid password. Must be at least 8 characters, with 1 uppercase, 1 lowercase, 1 digit, and 1 special character." << endl;
            } else {
            genPassword = password;
            break;
            }
        } else {
            genPassword = generateSaltStr(12);
            std::cout << "Generated Password: " << genPassword << std::endl;
            std::cout << "Please note down this password for login!" << std::endl;
            break;
        }
    } while(true);
    int userPoint = 0;
    if (enterPoint) {
        cout << "Enter user point: ";
        cin >> userPoint;

    }
    cout << "---------------------------" << endl;

    std::string hashedPassword = sha256(genPassword + genSalt);
    if(hashedPassword.empty()) {
        cout << "Error hashing password." << endl;
        return NULL;
    }
    genWalletId = sha256(genPassword + genSalt);
    cout << "Generated password: " << genPassword << endl;

    User * user = new User(FullName, userName, hashedPassword, userPoint, genSalt, genWalletId);
    return user;    
}

bool UserEditMenu(Admin * currentAdmin) {
    int userEditMenuOption;
    do {
        system("cls");
        printUserEditMenu();
        cin >> userEditMenuOption;

    } while (userEditMenuOption < 0 || userEditMenuOption>2);
    switch (userEditMenuOption)
    {
        case 0:
            return true;
        case 1: {
            cout << "Add user" << endl; 
            /* code */
            string saveParquetFileName = "../assets/users.parquet";

            User * user = enterUserInfoRegister(true);
            if (user == NULL)
            {
                cout << "Enter user info failed" << endl;
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
            break;
            
        }

        case 2: {
            cout << "Add user from csv" << endl; 
            // Nhap vao thong tin cua user muon tao qua cua so CLI
            std::string csvFileName = "../assets/user_info.csv";
            saveUserToDbFromCSV(csvFileName);


           
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

void AdminLoginMenu() {
    Admin * currentAdmin = new Admin();
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
    
            case 3:
                cout << "Wallet management" << endl; 
                /* code */
                break;
            case 4:
                cout << "Admin management" << endl; 
                /* code */
                break;
            
            default:
                break;
            }
            
        }

        // Admin page menu
    } else {
        cout  << "Login failed as admin" << endl;

    }

}

void changeuserinfo(std::string& filename, User *& currentUser) {
    if(!currentUser) {
        std::cerr << "Error: Current user is null!" << std::endl;
        return;
    }

    while (true){
        int subChoice;
        printchangeUserInfoMenu();
        cin >> subChoice;
        cin.ignore(); // Ignore the newline character left in the input buffer
        if(subChoice==1){
            string newFullName;
            cout << "Enter new full Name: ";
            getline(cin, newFullName);
            newFullName = trim(newFullName);
            if(newFullName.empty()){
                cout << "Error: Full name cannot be empty!" << std::endl;
                continue;
            }
            currentUser->setFullName(newFullName);
            std::map<std::string, std::string> updated_values={
                {"Fullname", currentUser->fullName()}
            };
            arrow::Status status = updateUserInfo(filename, currentUser, updated_values);
            if (!status.ok()) {
                cout << "Error updating user info: " << status.ToString() << endl;
                continue;
            }
            cout << "Full name changed successfully!" << endl;
        } else if(subChoice==2){
            string newPassword;
            cout << "Enter new password: ";
            getline(cin, newPassword);
            newPassword = trim(newPassword);
            if(!newPassword.empty()){
                string salt = currentUser->salt();
                string hashedPassword = sha256(newPassword + salt);
                currentUser->setPassword(hashedPassword);
                map<string, string> updated_values={
                    {"Password", hashedPassword},        
                    {"Salt", salt}
                };
                arrow::Status status = updateUserInfo(filename, currentUser, updated_values);
                if (!status.ok()) {
                    cout << "Error updating user info: " << status.ToString() << endl;
                    continue;
                }
                cout << "Password changed successfully!" << endl;
            }
            else{
                cout << "Password cannot be empty!" << endl;
            }
        } else if(subChoice==3){
            cout << "Back to main menu" << endl;
            cout << "DEBUG: End of changeuserinfo" << endl;
            break; // Exit the loop to go back to the main menu
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
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
            cout << "transaction history comming soon!" << endl;
        } else if(subChoice==2){
            cout << "transaction history comming soon!" << endl;
        } else if(subChoice==3){
            cout << "transaction history by time comming soon!" << endl;
        } else if(subChoice==4){
            cout << "Back to main menu" << endl;
            break; // Exit the loop to go back to the main menu
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

void UserLoginMenu(User *& currentUser) {
    while (true){
        int choice;
        cout << "\nUser: " << currentUser->accountName() << endl;
        printUserLoginMenu();
        cin >> choice;
        cin.ignore(); // Ignore the newline character left in the input buffer
        if(choice==1){
           printUserInfoFromDb(currentUser);
           std::string filename = "../assets/users.parquet";
           changeuserinfo(filename, currentUser);
        } else if(choice==2){
            eWallet(currentUser);
        } else if(choice==3){
            cout << "Logging out..." << endl;
            break; // Exit the loop to log out
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}


void userHomeMenu() {
    while (true){
        // system("cls");
        int choice;
        printuserHomeMenu();
        cin >> choice;
        cin.ignore(); // Ignore the newline character left in the input buffer
        shared_ptr<arrow::io::ReadableFile> infile;

        if(choice==1){
            User * currentUser = nullptr;
            loginUser(infile, currentUser);
        } else if(choice==2){
            // User * 
            User * new_user = enterUserInfoRegister();
            arrow::Status status = registerUser(new_user);
            if(!status.ok()) {
                cout << "Error registering user: " << status.ToString() << endl;
                return;
            }else {
                cout << "User registered successfully!" << endl;
            }
            UserLoginMenu(new_user);
            cout << "User registered successfully!" << endl;
        } else if(choice==3){
            cout << "Back to Main Menu..." << endl;
            break; // Exit the loop to go back to the main menu
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

void mainMenu() {
    while (true){
        system("cls");
        int choice;
        printmainMenu();
        cin >> choice;
        cin.ignore(); // Ignore the newline character left in the input buffer
        if(choice==1){
            userHomeMenu();
        } else if(choice==2){
            AdminLoginMenu();
        } else if(choice==3){
            cout << "Goodbye!..." << endl;
            exit(0); // Exit the loop to log out
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}