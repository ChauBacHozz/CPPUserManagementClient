#include "Menus.h"
#include "Admin.h"
#include "User.h"
#include "DbUtils.h"
#include <iostream>
#include "User.h"
#include <string>
#include "arrow/io/file.h"
#include "parquet/stream_reader.h"
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

User * enterUserInfoRegister(){
    system("cls");
    string fullName;
    string userName;
    string genPassword;
    string genSalt;
    string genWallet;
    cout << "ENTER USER INFO" << endl;
    cout << "---------------------------" <<endl;
    cout << "User fullname: ";
    cin.ignore();
    getline(cin, fullName);
    cout << "User username: ";
    cin >> userName;
    cout << "---------------------------" << endl;

    // Generate random password for first time register
    genPassword = generateSaltStr(12);
    genSalt = generateSaltStr();
    genWallet = sha256(genPassword + genSalt);
    cout << "Generated password: " << genPassword << endl;




    User * user = new User(fullName, userName, genPassword, 0, genSalt, genWallet);
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
            User * user1 = enterUserInfo();
            if (user1 == NULL)
            {
                cout << "Enter user info failed" << endl;
                break;
            }
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
                    printUserInfoFromDb();
                    
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
                
    
                // currentAdmin->createUser(user1->fullName(), user1->accountName(), user1->password(), user1->point());
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


void changeuserinfo(User *& currentUser) {
    while (true){
        int subChoice;
        cout << "\n--- Change Infomation ---\n" 
            << "1. Change Full Name\n" 
            << "2. Change Password\n" 
            << "3. Back\n";
        cout << "Enter your choice: ";
        cin >> subChoice;
        cin.ignore(); // Ignore the newline character left in the input buffer
        if(subChoice==1){
            string newFullName;
            cout << "Enter new full name: ";
            getline(cin, newFullName);
            currentUser->setFullName(newFullName);
            cout << "Full name changed successfully!" << endl;
        } else if(subChoice==2){
            string newPassword;
            cout << "Enter new password: ";
            getline(cin, newPassword);
            currentUser->setPassword(newPassword);
            cout << "Password changed successfully!" << endl;
        } else if(subChoice==3){
            cout << "Back to main menu" << endl;
            break; // Exit the loop to go back to the main menu
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

void eWallet(User *& currentUser) {
    while (true){
        int subChoice;
        cout << "\n--- E-Wallet ---\n" << "Balance: " << currentUser -> point() << " points" << endl;
        cout << "--------------------\n" << endl;
        cout << "1. Transfer Point\n" 
             << "2. Transaction history (all)\n" 
             << "3. Transaction history (by time)\n" 
             << "4. Back\n";
        cout << "Enter your choice: ";
        cin >> subChoice;
        cin.ignore(); // Ignore the newline character left in the input buffer
        if(subChoice==1){
            string dbWalletId;
            cout << "Enter wallet ID to transfer points: ";
            cin >> dbWalletId;
            cout << "transfer point comming soon!" << endl;
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
        cout << "\n--- User Menu ---\n" 
             << "1. User Info\n" 
             << "2. E-Wallet\n" 
             << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Ignore the newline character left in the input buffer
        if(choice==1){
           printUserInfoFromDb(currentUser);
           changeuserinfo(currentUser);
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


void userAuthMenu() {
    while (true){
        // system("cls");
        int choice;
        cout << "\n--- User Authentication ---\n" 
             << "1. Login\n" 
             << "2. Register\n" 
             << "3. Back\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Ignore the newline character left in the input buffer
        if(choice==1){
            shared_ptr<arrow::io::ReadableFile> infile;
            User * currentUser = nullptr;
            loginUser(infile, currentUser);
        } else if(choice==2){
            // User * 
            User * new_user = enterUserInfoRegister();
            UserLoginMenu(new_user);

        } else if(choice==3){
            cout << "Back to Main Menu..." << endl;
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}
void mainMenu() {
    while (true){
        system("cls");
        int choice;
        cout << "\n--- Main Menu ---\n" 
             << "1. User Authentication\n" 
             << "2. Admin Authentication\n" 
             << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Ignore the newline character left in the input buffer
        if(choice==1){
            userAuthMenu();
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