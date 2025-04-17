#include <iostream>
#include "User.h"
#include <string>
#include "arrow/io/file.h"
#include "parquet/stream_reader.h"


void login(std::shared_ptr<arrow::io::ReadableFile> infile){
    std::string userName;
    std::string userPassword;
    // std::string userName;
    std::cout << "Nhap vao ten nguoi dung: ";
    std::cin >> userName;
    std::cout << "Nhap vao mat khau: ";
    std::cin >> userPassword;

     
    PARQUET_ASSIGN_OR_THROW(
        infile,
        arrow::io::ReadableFile::Open("../assets/userdata.parquet"));
  
     parquet::StreamReader stream{parquet::ParquetFileReader::Open(infile)};
  
    std::string dbUserName;
    std::string dbUserPassword;
    int64_t dbUserPoint;
 
  
    while (!stream.eof() )
    {
        stream >> dbUserName >> dbUserPassword >> dbUserPoint >> parquet::EndRow;
        dbUserPoint = int(dbUserPoint);


        if ((dbUserName == userName) && (dbUserPassword == userPassword)) {
            User currentUser(userName, userPassword, dbUserPoint);
            currentUser.printUserInfo();
            break;
        }
    // ...
    }
}

int main () {
    std::shared_ptr<arrow::io::ReadableFile> infile;



    login(infile);


    return 0;
}