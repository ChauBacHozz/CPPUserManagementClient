#include <iostream>
#include <arrow/io/file.h>
#include <parquet/stream_reader.h>
#include "User.h"
#include <string>

int main () {
    std::shared_ptr<arrow::io::ReadableFile> infile;

    PARQUET_ASSIGN_OR_THROW(
        infile,
        arrow::io::ReadableFile::Open("D:/VS_Workspace/CppTemplate/assets/test.parquet"));

    parquet::StreamReader os{parquet::ParquetFileReader::Open(infile)};

    int64_t size;
    double price;

    while (!os.eof())
    {
        os >> size >> price >> parquet::EndRow;
        std::cout << "SIZE: " << size << " PRICE: " << price << std::endl;
    }
    
    std::string userName = "Nguyen Duc Phong";
    std::string userAccount = "Phong123";
    std::string userPassword = "ndPhongZ2222";
    int userPoint = 12;

    User tempUser(userName, userAccount, userPassword, userPoint);
    tempUser.printUserInfo();
    return 0;
}