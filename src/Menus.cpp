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
bool UserEditMenu(Admin * currentAdmin) {
    int userEditMenuOption;
    do {
        system("cls");
        printUserEditMenu();
        cin >> userEditMenuOption;

    } while (userEditMenuOption < 1 || userEditMenuOption>2);
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
        bool adminLoginMenuExit = false;
        while (!adminLoginMenuExit)
        {
            /* code */
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