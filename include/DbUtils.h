#pragma once
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <string>
#include <memory>

arrow::Status AppendParquetRow(std::string& filename);
