#include "DbUtils.h"
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <string>
#include <memory>



arrow::Status AppendParquetRow(std::string& filename) {
    // Mở file parquet
    std::shared_ptr<arrow::io::ReadableFile> infile;
    ARROW_ASSIGN_OR_RAISE(
        infile,
        arrow::io::ReadableFile::Open(filename)
    );
    // Tạo parquet reader
    std::unique_ptr<parquet::arrow::FileReader> fileReader;
    ARROW_RETURN_NOT_OK(parquet::arrow::OpenFile(
        infile,
        arrow::default_memory_pool(),
        &fileReader
    ));
    
}
