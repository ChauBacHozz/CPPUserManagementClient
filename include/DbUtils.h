#pragma once
#ifndef DBUTILS_H
#define DBUTILS_H
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <string>
#include <memory>

arrow::Status AppendUserParquetRow(std::string& filename, std::string& FullName, std::string& UserName, std::string& UserPassword, std::string& Salt, int Point, std::string& WalletId);
#endif