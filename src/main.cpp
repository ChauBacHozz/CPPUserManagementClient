#include <iostream>
#include "User.h"
#include <string>
#include "arrow/io/file.h"
#include "parquet/stream_reader.h"

int main () {
    std::shared_ptr<arrow::io::ReadableFile> infile;

    PARQUET_ASSIGN_OR_THROW(
       infile,
       arrow::io::ReadableFile::Open("../assets/test.parquet"));
 
    parquet::StreamReader stream{parquet::ParquetFileReader::Open(infile)};
 
    int64_t s;
    double p;

 
    while (!stream.eof() )
    {
        stream >> s >> p >> parquet::EndRow;
        std::cout << "SIZE: " << s << " PRICE: " << p << std::endl;
       // ...
    }

    return 0;
}