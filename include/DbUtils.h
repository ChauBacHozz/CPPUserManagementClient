#pragma once
#ifndef DBUTILS_H
#define DBUTILS_H
#include <arrow/api.h>
#include <arrow/io/api.h>
#include "arrow/io/file.h"
#include "User.h"
#include <string>
#include <memory>
#include <vector>


arrow::Status AppendUserParquetRow(std::string& filename, std::string& FullName, std::string& UserName, std::string& UserPassword, std::string& Salt, int Point, std::string& WalletId);
std::vector<std::vector<std::string>> ReadCSV(const std::string& filename);
bool saveUserToDbFromCSV(std::string& filename);
void loginUser(std::shared_ptr<arrow::io::ReadableFile> infile, User *& currentUser);
arrow::Status printUserInfoFromDb();



#endif