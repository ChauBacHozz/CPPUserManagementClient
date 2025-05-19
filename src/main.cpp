#include <iostream>
#include "User.h"
#include "Admin.h"
#include <string>
#include "arrow/io/file.h"
#include "parquet/stream_reader.h"
#include <openssl/sha.h>
#include "DbUtils.h"
#include "Menus.h"
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






int main () {
    while (true)
    {
        AdminLoginMenu();
    }
    




    return 0;
}
