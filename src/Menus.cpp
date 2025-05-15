#include "Menus.h"
#include "Admin.h"
#include "User.h"
#include "DbUtils.h"
#include <iostream>
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


    User * user = new User(fullName, userName, password, point);
    return user;    
}
void UserEditMenu(Admin * currentAdmin) {
    int userEditMenuOption;
    do {
        system("cls");
        printUserEditMenu();
        cin >> userEditMenuOption;

    } while (userEditMenuOption < 0 || userEditMenuOption>2);
    switch (userEditMenuOption)
    {
        case 0:
            cout << "Exiting!!!" << endl; 
            break;
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

}

bool loginAdmin(Admin *& currentAdmin) {
    string adminName;
    string adminPassword;
    // string userName;
    cout << "Nhap vao ten admin: ";
    cin >> adminName;
    cout << "Nhap vao mat khau admin: ";
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
            cout << "Exiting!!!" << endl; 
            break;
        case 1:
            cout << "User Listing" << endl; 
            /* code */
            break;
        case 2: {
            cout << "User Editing" << endl; 
            // Nhap vao thong tin cua user muon tao qua cua so CLI
            UserEditMenu(currentAdmin);
            // User * user1 = enterUserInfo();
            // if (user1 == NULL)
            // {
            //     cout << "Enter user info failed" << endl;
            //     break;
            // }
            

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
        

        // Admin page menu
    } else {
        cout  << "Login failed as admin" << endl;

    }

}