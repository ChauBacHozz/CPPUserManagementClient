#include <iostream>
#include "User.h"
#include "Admin.h"
#include <string>
#include "arrow/io/file.h"
#include "parquet/stream_reader.h"
#include <openssl/sha.h>
using namespace std;


// void loginUser(shared_ptr<arrow::io::ReadableFile> infile, User *& currentUser){
//     string userName;
//     string userPassword;
//     // string userName;
//     cout << "Nhap vao ten nguoi dung: ";
//     cin >> userName;
//     cout << "Nhap vao mat khau: ";
//     cin >> userPassword;

     
//     PARQUET_ASSIGN_OR_THROW(
//         infile,
//         arrow::io::ReadableFile::Open("../assets/userdata.parquet"));
  
//      parquet::StreamReader stream{parquet::ParquetFileReader::Open(infile)};
  
//     string dbUserName;
//     string dbUserPassword;
//     int64_t dbUserPoint;
//     Thuan da o day;
//     Thuan lai o day
 
  
//     while (!stream.eof() )
//     {
//         stream >> dbUserName >> dbUserPassword >> dbUserPoint >> parquet::EndRow;
//         dbUserPoint = int(dbUserPoint);


//         if ((dbUserName == userName) && (dbUserPassword == userPassword)) {
//             cout << "Check" << endl;
//             currentUser = new User(userName, userPassword, dbUserPoint);
//             return;
//         }

//     }
// }
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

int main () {
    Admin * currentAdmin = new Admin();
    shared_ptr<arrow::io::ReadableFile> infile;
    
    
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
            return 0;
            break;
        case 1:
            cout << "User Listing" << endl; 
            /* code */
            break;
        case 2: {
            cout << "User Editing" << endl; 
            // Nhap vao thong tin cua user muon tao qua cua so CLI

            User * user1 = enterUserInfo();
            if (user1 == NULL)
            {
                cout << "Enter user info failed" << endl;
                break;
            }
            

            currentAdmin->createUser(user1->fullName(), user1->accountName(), user1->password(), user1->point());
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


    return 0;
}
