#include "DbUtils.h"
#include "Encrypt.h"
#include <arrow/api.h>
#include <arrow/io/api.h>
#include "arrow/io/file.h"
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include "parquet/stream_reader.h"
#include "Menus.h"
#include "User.h"
#include <arrow/pretty_print.h>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <set>
#include <openssl/sha.h>

arrow::Status getTableFromFile(std::string& filename, std::shared_ptr<arrow::Table>& existing_table) {
    // Mở file parquet
    std::shared_ptr<arrow::io::ReadableFile> infile;
    ARROW_ASSIGN_OR_RAISE(
        infile,
        arrow::io::ReadableFile::Open(filename)
    );
    // Tạo parquet reader
    std::unique_ptr<parquet::arrow::FileReader> fileReader;
    ARROW_ASSIGN_OR_RAISE(fileReader, parquet::arrow::OpenFile(infile, arrow::default_memory_pool()));
    ARROW_RETURN_NOT_OK(fileReader->ReadTable(&existing_table));

    return arrow::Status::OK();
}

arrow::Status AppendUserParquetRow(std::string& filename, 
                                   std::string& FullName, 
                                   std::string& UserName, 
                                   std::string& UserPassword, 
                                   std::string& Salt, 
                                   int Point, 
                                   std::string& WalletId) {

    // Đọc bảng dữ liệu
    std::shared_ptr<arrow::Table> existing_table;
    getTableFromFile(filename, existing_table);

    // Lấy ra thông tin schema từ table
    std::shared_ptr<arrow::Schema> schema = existing_table->schema();

    // Tạo builder cho dòng mới 
    arrow::StringBuilder bdFullName;
    arrow::StringBuilder bdUserName;
    arrow::StringBuilder bdUserPassword;
    arrow::StringBuilder bdUserSalt;
    arrow::Int64Builder bdUserPoint;
    arrow::StringBuilder bdWalletId;
    
    // Thêm giá trị mới vào các builder
    ARROW_RETURN_NOT_OK(bdFullName.Append(FullName));
    ARROW_RETURN_NOT_OK(bdUserName.Append(UserName));
    ARROW_RETURN_NOT_OK(bdUserPassword.Append(UserPassword));
    ARROW_RETURN_NOT_OK(bdUserSalt.Append(Salt));
    ARROW_RETURN_NOT_OK(bdUserPoint.Append(Point));
    ARROW_RETURN_NOT_OK(bdWalletId.Append(WalletId));
    
    // Chuyển builder về dạng array
    std::shared_ptr<arrow::Array> arrFullName;
    std::shared_ptr<arrow::Array> arrUserName;
    std::shared_ptr<arrow::Array> arrUserPassword;
    std::shared_ptr<arrow::Array> arrSalt;
    std::shared_ptr<arrow::Array> arrPoint;
    std::shared_ptr<arrow::Array> arrWalletId;

    ARROW_RETURN_NOT_OK(bdFullName.Finish(&arrFullName));
    ARROW_RETURN_NOT_OK(bdUserName.Finish(&arrUserName));
    ARROW_RETURN_NOT_OK(bdUserPassword.Finish(&arrUserPassword));
    ARROW_RETURN_NOT_OK(bdUserSalt.Finish(&arrSalt));
    ARROW_RETURN_NOT_OK(bdUserPoint.Finish(&arrPoint));
    ARROW_RETURN_NOT_OK(bdWalletId.Finish(&arrWalletId));

    // Chuyển thành dạng bảng
    std::vector<std::shared_ptr<arrow::Array>> new_arrays = {arrFullName, 
                                                             arrUserName, 
                                                             arrUserPassword, 
                                                             arrSalt, 
                                                             arrPoint, 
                                                             arrWalletId};
    std::shared_ptr<arrow::Table> new_table = arrow::Table::Make(schema, new_arrays);

    // Ghép nối bảng cũ vào bảng mới
    std::vector<std::shared_ptr<arrow::Table>> table_arr = {existing_table, new_table};
    ARROW_ASSIGN_OR_RAISE(
        std::shared_ptr<arrow::Table> combined_table,
        arrow::ConcatenateTables(table_arr)
    );

    // Tạo file output
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    ARROW_ASSIGN_OR_RAISE(outfile, arrow::io::FileOutputStream::Open(filename));
    
    // Tạo file parquet mới từ bảng đã ghép
    parquet::WriterProperties::Builder props_builder;
    std::shared_ptr<parquet::WriterProperties> props = props_builder.build();
    int64_t chunk_size = 1024; // hoặc giá trị phù hợp
    ARROW_RETURN_NOT_OK(parquet::arrow::WriteTable(
        *combined_table,
        arrow::default_memory_pool(),
        outfile,
        chunk_size,
        props
    ));
    ARROW_RETURN_NOT_OK(outfile->Close());

    return arrow::Status::OK();
}

arrow::Status AppendBatchUserParquetRows(std::string& filename,
                                        std::vector<std::string>& FullNameArr,
                                        std::vector<std::string>& UserNameArr,
                                        std::vector<std::string>& UserPasswordArr,
                                        std::vector<std::string>& SaltArr,
                                        std::vector<int64_t>& PointArr,
                                        std::vector<std::string>& WalletIdArr
                                        ) 
{
    // Đọc bảng dữ liệu
    std::shared_ptr<arrow::Table> existing_table;
    ARROW_RETURN_NOT_OK(getTableFromFile(filename, existing_table));

    // Lấy ra thông tin schema từ table
    std::shared_ptr<arrow::Schema> schema = existing_table->schema();

    // Tạo builder cho dòng mới 
    arrow::StringBuilder bdFullName;
    arrow::StringBuilder bdUserName;
    arrow::StringBuilder bdUserPassword;
    arrow::StringBuilder bdUserSalt;
    arrow::Int64Builder bdUserPoint;
    arrow::StringBuilder bdWalletId;
    
    // Thêm giá trị mới vào các builder
    for (int i = 0; i < FullNameArr.size(); i++) {
        ARROW_RETURN_NOT_OK(bdFullName.Append(FullNameArr[i]));
        ARROW_RETURN_NOT_OK(bdUserName.Append(UserNameArr[i]));
        ARROW_RETURN_NOT_OK(bdUserPassword.Append(UserPasswordArr[i]));
        ARROW_RETURN_NOT_OK(bdUserSalt.Append(SaltArr[i]));
        ARROW_RETURN_NOT_OK(bdUserPoint.Append(PointArr[i]));
        ARROW_RETURN_NOT_OK(bdWalletId.Append(WalletIdArr[i]));
    }
    
    // Chuyển builder về dạng array
    std::shared_ptr<arrow::Array> arrFullName;
    std::shared_ptr<arrow::Array> arrUserName;
    std::shared_ptr<arrow::Array> arrUserPassword;
    std::shared_ptr<arrow::Array> arrSalt;
    std::shared_ptr<arrow::Array> arrPoint;
    std::shared_ptr<arrow::Array> arrWalletId;

    ARROW_RETURN_NOT_OK(bdFullName.Finish(&arrFullName));
    ARROW_RETURN_NOT_OK(bdUserName.Finish(&arrUserName));
    ARROW_RETURN_NOT_OK(bdUserPassword.Finish(&arrUserPassword));
    ARROW_RETURN_NOT_OK(bdUserSalt.Finish(&arrSalt));
    ARROW_RETURN_NOT_OK(bdUserPoint.Finish(&arrPoint));
    ARROW_RETURN_NOT_OK(bdWalletId.Finish(&arrWalletId));

    // Chuyển thành dạng bảng
    std::vector<std::shared_ptr<arrow::Array>> new_arrays = {arrFullName, arrUserName, arrUserPassword, arrSalt, arrPoint, arrWalletId};
    std::shared_ptr<arrow::Table> new_table = arrow::Table::Make(schema, new_arrays);

    // Ghép nối bảng cũ vào bảng mới
    std::vector<std::shared_ptr<arrow::Table>> table_arr = {existing_table, new_table};
    ARROW_ASSIGN_OR_RAISE(
        std::shared_ptr<arrow::Table> combined_table,
        arrow::ConcatenateTables(table_arr)
    );

    // Tạo file output
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    ARROW_ASSIGN_OR_RAISE(outfile, arrow::io::FileOutputStream::Open(filename));
    
    // Tạo file parquet mới từ bảng đã ghép
    parquet::WriterProperties::Builder props_builder;
    std::shared_ptr<parquet::WriterProperties> props = props_builder.build();
    int64_t chunk_size = 1024; // hoặc giá trị phù hợp
    ARROW_RETURN_NOT_OK(parquet::arrow::WriteTable(
        *combined_table,
        arrow::default_memory_pool(),
        outfile,
        chunk_size,
        props
    ));
    ARROW_RETURN_NOT_OK(outfile->Close());

    return arrow::Status::OK();

}
std::vector<int64_t> convertToIntVector(const std::vector<std::string>& strVec) {
    std::vector<int64_t> intVec;
    intVec.reserve(strVec.size());  // Tối ưu hiệu năng

    for (const auto& str : strVec) {
        try {
            intVec.push_back(std::stoi(str));
        } catch (const std::invalid_argument& e) {
            std::cerr << "Không thể chuyển '" << str << "' sang số nguyên\n";
            intVec.push_back(0);  // Hoặc xử lý theo ý bạn
        } catch (const std::out_of_range& e) {
            std::cerr << "Giá trị '" << str << "' vượt quá giới hạn của int\n";
            intVec.push_back(0);  // Hoặc xử lý theo ý bạn
        }
    }

    return intVec;
}
std::vector<std::vector<std::string>> ReadCSV(const std::string& filename) {
    std::vector<std::vector<std::string>> table;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Không thể mở file: " << filename << std::endl;
        return table;
    }

    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        table.push_back(row);
    }

    file.close();
    return table;
}
std::vector<std::vector<std::string>> TransposeTable(const std::vector<std::vector<std::string>>& table) {
    if (table.empty()) return {};

    size_t numRows = table.size();
    size_t numCols = table[0].size();
    std::vector<std::vector<std::string>> transposed(numCols, std::vector<std::string>(numRows));

    for (size_t i = 0; i < numRows; ++i) {
        for (size_t j = 0; j < table[i].size(); ++j) {
            transposed[j][i] = table[i][j];
        }
    }

    return transposed;
}
bool saveUserToDbFromCSV(std::string& filename) {
    std::vector<std::vector<std::string>> userInfoTable = ReadCSV(filename);
    userInfoTable = TransposeTable(userInfoTable);
    
    // Đổi cột cuối cùng trong table sang kiểu int64_t
    std::vector<int64_t> userPointVec = convertToIntVector(userInfoTable[3]);
    std::vector<std::string> saltArr;
    std::vector<std::string> hashedParrwordArr;
    std::vector<std::string> walletIdArr;
    for (int i = 0; i < userPointVec.size(); i++)
    {
        std::string salt = generateSaltStr();
        std::string hashedParrword = sha256(userInfoTable[2][i] + salt);
        std::string walletId = sha256(hashedParrword + salt);
        saltArr.push_back(salt);
        hashedParrwordArr.push_back(hashedParrword);
        walletIdArr.push_back(walletId);
    }
    
    std::string saveParquetFileName = "../assets/users.parquet";
    AppendBatchUserParquetRows(saveParquetFileName, 
        userInfoTable[0], 
        userInfoTable[1], 
        hashedParrwordArr, 
        saltArr,
        userPointVec,
        walletIdArr);
    return true;
}

std::string TruncateString(const std::string& s, size_t max_len = 15) {
    if (s.length() <= max_len) return s;
    return s.substr(0, max_len) + "...";
}

void PrintTableLikeCLI(const std::shared_ptr<arrow::Table>& table, std::vector<int> columns_orders) {
    const int col_width = 15;

    // Print headers
    for (const int& i : columns_orders) {
        std::string header = table->schema()->field(i)->name();
        std::cout << std::setw(col_width) << TruncateString(header) << " | ";
    }
    std::cout << "\n";

    // Print separator
    for (const int& col : columns_orders) {
        std::cout << std::string(col_width, '-') << " | ";
    }
    std::cout << "\n";

    // Print rows
    int64_t num_rows = table->num_rows();
    for (int64_t row = 0; row < num_rows; ++row) {
        for (const int& col : columns_orders) {
            const auto& chunked_array = table->column(col);
            int64_t offset = 0;
            for (const auto& chunk : chunked_array->chunks()) {
                if (row < offset + chunk->length()) {
                    auto scalar_result = chunk->GetScalar(row - offset);
                    if (scalar_result.ok()) {
                        std::string cell = scalar_result.ValueOrDie()->ToString();
                        std::cout << std::setw(col_width) << TruncateString(cell) << " | ";
                    } else {
                        std::cout << std::setw(col_width) << "ERR" << " | ";
                    }
                    break;
                }
                offset += chunk->length();
            }
        }
        std::cout << "\n";
    }
}

arrow::Status printUserInfoFromDb() {
    std::string filename = "../assets/users.parquet";
    std::shared_ptr<arrow::Table> table;
    ARROW_RETURN_NOT_OK(getTableFromFile(filename, table));

    std::vector<int> column_orders = {0,1,2};
    PrintTableLikeCLI(table, column_orders);
    return arrow::Status::OK();
}
void logFailedLogin(std::string& userName) {
    std::ofstream logFile("login_failures.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << "Failed login attempt: " << userName << ", failed to login 3 times. Account temporarily locked." << std::endl;
        logFile.close();
    } else {
        std::cerr << "Unable to open log file." << std::endl;
    }
}
void loginUser(std::shared_ptr<arrow::io::ReadableFile> infile, User *& currentUser){
    std::string userName;
    std::cout << "User name: ";
    std::cin >> userName;
    
    try {
        PARQUET_ASSIGN_OR_THROW(
           infile,
            arrow::io::ReadableFile::Open("../assets/users.parquet"));
    } catch (const arrow::Status& status) {
            std::cerr << "Error opening file: " << status.ToString() << std::endl;
            currentUser = nullptr;
            return;        
        }

    parquet::StreamReader stream{parquet::ParquetFileReader::Open(infile)};

    std::string dbFullName;
    std::string dbUserName;
    std::string dbSalt;
    std::string dbWalletId;
    std::string dbhasdedPassword;
    int64_t dbUserPoint;
    bool userfound = false;
 
  
    while (!stream.eof() ){ 
        stream >> dbFullName >> dbUserName >> dbhasdedPassword >> dbSalt >> dbUserPoint >> dbWalletId >> parquet::EndRow;

        if (userName == dbUserName) {
            userfound = true;
            break;
        }   
    }
    if (!userfound) {
        std::cout << "Login failed! (User invalid)" << std::endl;
        currentUser = nullptr;
    }
    int failedLoginCount = 0;
    while (failedLoginCount < 3) {
        std::string userpassword;
        std::cout << "Password: ";
        std::cin >> userpassword;

        std::string hashedPassword = sha256(userpassword + dbSalt);
        if (hashedPassword == dbhasdedPassword) {
            std::cout << "Login successful!" << std::endl;
            currentUser = new User(dbFullName, dbUserName, dbhasdedPassword, dbUserPoint, dbSalt, dbWalletId);
            UserLoginMenu(currentUser);
            break;
        } else {
            failedLoginCount++;
            std::cout << "Login failed! (Password invalid)" << std::endl;
            if (failedLoginCount == 3) {
                logFailedLogin(userName);
                std::cout << "Account temporarily locked due to multiple failed login attempts." << std::endl;
                currentUser = nullptr;
                return;
            }
        }
    }
}

arrow::Status registerUser(User *& user) {
    std::string filename = "../assets/users.parquet";
    // Auto set user point = 0 if register
    user->setPoint(0);
    std::string fullName = user->fullName();
    std::string accountName = user->accountName();
    std::string password = user->password();
    std::string salt = user->salt();
    int point = user->point();
    std::string wallet = user->wallet();

    arrow::Status resultRegisterUser = AppendUserParquetRow(filename,
                                                            fullName,
                                                            accountName,
                                                            password,
                                                            salt,
                                                            point,
                                                            wallet);
    return arrow::Status::OK();
}

arrow::Status findUserrow(const std::shared_ptr<arrow::Table>& table, 
                          const std::string& userName,
                          int64_t& rowToUpdate) {
                            
    auto userNameColumn = table->GetColumnByName("UserName");
    if(!userNameColumn) {
        std::cerr << "Column UserName not found" << std::endl;
        return arrow::Status::Invalid("Column UserName not found");
    }
    if(userNameColumn->type() -> id() != arrow::Type::STRING) {
        std::cerr << "Column UserName is not a string" << std::endl;
        return arrow::Status::Invalid("Column UserName is not a string");
    }

    rowToUpdate = -1;
    int64_t rowIdx = 0;
    for(const auto& chunk : userNameColumn->chunks()) {
        auto string_column = std::static_pointer_cast<arrow::StringArray>(chunk);
        for (int64_t i = 0; i < string_column->length(); i++, rowIdx++) {
            if(!string_column->IsNull(i) && string_column->GetString(i) == userName) {
                rowToUpdate = rowIdx;
                return arrow::Status::OK();
            }
        }
    }
    std::cerr << "User " << userName << " not found" << std::endl;
    return arrow::Status::Invalid("User " + userName + " not found!");
}

arrow::Status checkRequiredColumns(const std::shared_ptr<arrow::Table>& table, 
                                   const std::set<std::string>& requires_columns,
                                   const std::map<std::string, 
                                   arrow::Type::type>& expected_types) { 
    for(const auto& col : requires_columns) {
        auto column = table->GetColumnByName(col);
        if(!column) {
            std::cerr << "Required column " << col << " not found" << std::endl;
            return arrow::Status::Invalid("Column " + col + " not found");
        }
        auto it = expected_types.find(col);
        if(it != expected_types.end() && column->type()->id() != it->second) {
            std::cerr << "Column " << col << " has is not of expected type" << std::endl;
            return arrow::Status::Invalid("Column " + col + " is not of expected type");
        }
    }
    return arrow::Status::OK();
}

arrow::Status updateUserInfo(const std::string& filename,
                             User*& user,
                            const std::map<std::string, 
                            std::string>& updated_values,
                            bool allow_point_update) {
    //Kiểm tra đầu vào
    if(filename.empty()) {
        std::cerr << "Filename is empty" << std::endl;
        return arrow::Status::Invalid("Filename is empty");
    }
    if(!user) {
        std::cerr << "User is null" << std::endl;
        return arrow::Status::Invalid("User is null");
    }
    if(updated_values.find("WalletId") != updated_values.end()) {
        std::cerr << "WalletId cannot be update!" << std::endl;
        return arrow::Status::Invalid("WalletId cannot be update!");
    }
    if(!allow_point_update && updated_values.find("Point") != updated_values.end()) {
        std::cerr << "Point cannot be update!" << std::endl;
        return arrow::Status::Invalid("Point cannot be update!");
    }

    std::cout << "Starting updateUserInfo for file: " << filename << ", user: " << user->accountName() << std::endl;
    
    // Đọc bảng từ file parquet
    std::shared_ptr<arrow::Table> table;
    ARROW_RETURN_NOT_OK(getTableFromFile(const_cast<std::string&>(filename), table));
        
    std::cout << "Table read successfully, num_rows: " << table->num_rows() << ", num_columns: " << table->num_columns() << std::endl;

    // Kiểm tra các cột cần cập nhật
    std::set<std::string> requires_columns = {"UserName"};
    std::map<std::string, arrow::Type::type> expected_types = {
        {"UserName", arrow::Type::STRING},
        {"FullName", arrow::Type::STRING},
        {"UserPassword", arrow::Type::STRING},
        {"Salt", arrow::Type::STRING},
        {"Point", arrow::Type::INT64},
        {"WalletId", arrow::Type::STRING}
    };
    for(const auto& [col, _] : updated_values) {
        requires_columns.insert(col);
    }
    ARROW_RETURN_NOT_OK(checkRequiredColumns(table, requires_columns, expected_types));
    // Tìm hàng cần cập nhật
    int64_t rowToUpdate = -1;
    ARROW_RETURN_NOT_OK(findUserrow(table, user->accountName(), rowToUpdate));
    std::cout << "Found user at row: " << rowToUpdate << std::endl;

    //Tạo cột mới
    std::vector<std::shared_ptr<arrow::Array>> newColumns;
    for(int colIdx = 0; colIdx < table->num_columns(); ++colIdx) {
        auto column = table->column(colIdx);
        auto fieldName = table->field(colIdx)->name();
        std::shared_ptr<arrow::Array> newColumn;
        //Nếu cột cần cập nhật
        auto it = updated_values.find(fieldName);
        if(it != updated_values.end()) {
            if(fieldName == "Point") {
                arrow::Int64Builder builder;
                int64_t value;
                try
                {
                    value = std::stoll(it->second);
                }
                catch(...){
                    std::cerr << "Invalid integer value for column Point: " << it->second << std::endl;
                    return arrow::Status::Invalid("Invalid integer value for column Point");
                }
                int64_t currentRow = 0;
                for (const auto& chunk : column->chunks()) {
                    auto int64_array = std::static_pointer_cast<arrow::Int64Array>(chunk);
                    for(int64_t i = 0; i < int64_array->length(); i++, currentRow++) {
                        if(rowToUpdate >= 0 && rowToUpdate < table->num_rows() && currentRow == rowToUpdate) {
                            ARROW_RETURN_NOT_OK(builder.Append(value));
                        } else {
                            ARROW_RETURN_NOT_OK(builder.Append(int64_array->Value(i)));
                        }
                    }
                }
                ARROW_RETURN_NOT_OK(builder.Finish(&newColumn));
            } else {
                arrow::StringBuilder builder;
                int64_t currentRow = 0;
                for (const auto& chunk : column->chunks()) {
                    auto string_array = std::static_pointer_cast<arrow::StringArray>(chunk);
                    for(int64_t i = 0; i < string_array->length(); i++, currentRow++) {
                        if(rowToUpdate >= 0 && rowToUpdate < table->num_rows() && currentRow == rowToUpdate) {
                            ARROW_RETURN_NOT_OK(builder.Append(it->second));
                        } else if(string_array->IsNull(i)) {
                            ARROW_RETURN_NOT_OK(builder.AppendNull());
                        } else {
                            ARROW_RETURN_NOT_OK(builder.Append(string_array->GetString(i)));
                        }
                    }
                }
                ARROW_RETURN_NOT_OK(builder.Finish(&newColumn));}
        } else {
            arrow::ArrayVector chunks = column->chunks();
            if(chunks.size() == 1) {
                newColumn = chunks[0];
            } else {
                ARROW_ASSIGN_OR_RAISE(newColumn, arrow::Concatenate(chunks));
            }
        }
        newColumns.push_back(newColumn);
    }
        std::cout << "Updating columns created: " << filename << std::endl; 

        //Tạo bảng mới
        auto newTable = arrow::Table::Make(table->schema(), newColumns);
        //Ghi bảng mới vào file parquet
        std::shared_ptr<arrow::io::FileOutputStream> outfile;
        ARROW_ASSIGN_OR_RAISE(outfile, arrow::io::FileOutputStream::Open(filename));
        parquet::WriterProperties::Builder props_builder;
        std::shared_ptr<parquet::WriterProperties> props = props_builder.build();
        ARROW_RETURN_NOT_OK(parquet::arrow::WriteTable(
            *newTable,
            arrow::default_memory_pool(),
            outfile,
            1024,
            props
        ));
        ARROW_RETURN_NOT_OK(outfile->Close());
        std::cout << "Successfully updated user info in file: " << filename << std::endl;
        return arrow::Status::OK();
    }