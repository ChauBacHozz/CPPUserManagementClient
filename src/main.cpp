#include <iostream>
#include "User.h"
#include <string>
#include "arrow/io/file.h"
#include "parquet/stream_reader.h"

int main () {
    std::shared_ptr<arrow::io::ReadableFile> infile;

    PARQUET_ASSIGN_OR_THROW(
       infile,
       arrow::io::ReadableFile::Open("../assets/account.parquet"));
 
    parquet::StreamReader stream{parquet::ParquetFileReader::Open(infile)};
 
    std::string name;
    std::string age;
    std::string point;

 
    while (!stream.eof() )
    {
        stream >> name >> age >> point >> parquet::EndRow;
        std::cout << "Name: " << name << " Age: " << age << " Point: " << point << std::endl;
       // ...
    }

    return 0;
}