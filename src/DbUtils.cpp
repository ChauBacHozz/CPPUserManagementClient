#include "DbUtils.h"
#include "Encrypt.h"
#include "JsonUtils.h"
#include <arrow/api.h>
#include <arrow/io/api.h>
#include "arrow/io/file.h"
#include "arrow/io/file.h"
#include "Menus.h"
#include "User.h"
#include <arrow/pretty_print.h>
#include <arrow/result.h>
#include <arrow/util/macros.h>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <set>
#include <openssl/sha.h>
#include <random>
#include <filesystem>
#include <chrono>
#include <arrow/api.h>
#include <transaction_utils.h>
#include <UserStatus.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <parquet/stream_reader.h>
#include <parquet/stream_writer.h>

//Hàn đọc trạng thái User Status
bool readUserStatusFromFile(std::string& user, UserStatus& status) {
    std::shared_ptr<arrow::io::ReadableFile> infileStatus;
    auto fileStatus = arrow::io::ReadableFile::Open("../assets/userstatus.parquet");
    if (!fileStatus.ok()) {
        std::cerr << "Error opening file: " << fileStatus.status().ToString() << std::endl;
        return false;
    }
    infileStatus = fileStatus.ValueOrDie();
    std::unique_ptr<parquet::ParquetFileReader> reader = parquet::ParquetFileReader::Open(infileStatus);
    parquet::StreamReader stream(std::move(reader));
    std::string dbUser, dbIsGeneratedPassword, dbFailedLogin, dbDeleteUser, dbDate;
    bool userFound = false;
    while (!stream.eof()) {
        stream >> dbUser >> dbIsGeneratedPassword >> dbFailedLogin >> dbDeleteUser >> dbDate >> parquet::EndRow;
        if (dbUser.empty()) {
            std::cerr << "Warning: Empty user in userstatus.parquet row, skipping ..." << std::endl;
            continue;
        }
        if (dbUser == user) {
            userFound = true;
            status.setgetUser(dbUser);
            status.setIsGeneratedPassword(dbIsGeneratedPassword);
            status.setFailedLogin(dbFailedLogin);
            status.setDeleteUser(dbDeleteUser);
            status.setDate(dbDate);
            break;
        }
    }
    if (!userFound) {
        // Khởi tạo trạng thái mặc định nếu không tìm thấy người dùng
        std::string defaultStatus = "false";
        std::string defaultDate = "N/A";
        std::string fileStatus = "../assets/userstatus.parquet";
        status = UserStatus(user, defaultStatus, defaultStatus, defaultStatus, defaultDate);
        // Thêm dòng mới vào file userstatus.parquet
        appendUserStatusRow(fileStatus, user, defaultStatus, defaultStatus, defaultStatus, defaultDate);
        std::cerr << "User: " << user << " not found in userstatus.parquet, initialized with default values!" << std::endl;
        return false;
    }
    return true;
}

//Hàm thêm một dòng mới vào file userstatus.parquet
bool appendUserStatusRow(std::string& filename,
                         std::string& user,
                         std::string& isGeneratedPassword,
                         std::string& failedLogin,
                         std::string& deleteUser,
                         std::string& date) {
    // Đọc bảng hiện tại (nếu có)
    std::shared_ptr<arrow::Table> existing_table;
    getTableFromFile(filename, existing_table);

    // Tạo builder cho dòng mới
    arrow::StringBuilder bdUser, bdIsGeneratedPassword, bdFailedLogin, bdDeleteUser, bdDate;
    bdUser.Append(user);
    bdIsGeneratedPassword.Append(isGeneratedPassword);
    bdFailedLogin.Append(failedLogin);
    bdDeleteUser.Append(deleteUser);
    bdDate.Append(date);

    // Chuyển builder về dạng array
    std::shared_ptr<arrow::Array> arrUser, arrIsGeneratedPassword, arrFailedLogin, arrDeleteUser, arrDate;
    bdUser.Finish(&arrUser);
    bdIsGeneratedPassword.Finish(&arrIsGeneratedPassword);
    bdFailedLogin.Finish(&arrFailedLogin);
    bdDeleteUser.Finish(&arrDeleteUser);
    bdDate.Finish(&arrDate);

    // Tạo schema cho bảng userstatus.parquet
    auto schema = arrow::schema({
        arrow::field("User", arrow::utf8()),
        arrow::field("isGeneratedPassword", arrow::utf8()),
        arrow::field("failedLogin", arrow::utf8()),
        arrow::field("deleteUser", arrow::utf8()),
        arrow::field("Date", arrow::utf8())
    });

    auto new_table = arrow::Table::Make(schema, {arrUser, 
                                                 arrIsGeneratedPassword, 
                                                 arrFailedLogin, 
                                                 arrDeleteUser, 
                                                 arrDate});
    
    // Nếu bảng hiện tại không có, tạo bảng mới
    std::shared_ptr<arrow::Table> final_table;
    if (existing_table && existing_table->num_rows() > 0) {
        // Nối bảng cũ và bảng mới
        arrow::Result<std::shared_ptr<arrow::Table>> concat_result = arrow::ConcatenateTables({existing_table, new_table});
        if (!concat_result.ok()) {
            std::cerr << "Error concatenating tables: " << concat_result.status().ToString() << std::endl;
            return false;
        }
        final_table = concat_result.ValueOrDie();
    } else {
        final_table = new_table;
    }

    // Ghi lại vào file Parquet
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    arrow::Status outst = arrow::io::FileOutputStream::Open(filename).Value(&outfile);
    if (!outst.ok()) {
        std::cerr << "Error opening output file: " << outst.ToString() << std::endl;
        return false;
    }
    parquet::WriterProperties::Builder props_builder;
    std::shared_ptr<parquet::WriterProperties> props = props_builder.build();
    arrow::Status write_st = parquet::arrow::WriteTable(*final_table, arrow::default_memory_pool(), outfile, 1024, props);
    if (!write_st.ok()) {
        std::cerr << "Error writing table: " << write_st.ToString() << std::endl;
        return false;
    }
    outfile->Close();
    return true;
}

// Hàm ghi trạng thái User Status vào file
bool updateUserStatusRow(const std::string& filename, const std::string& userName, 
                        const std::map<std::string, std::string>& status_update) {
    // Đọc bảng hiện tại
    std::shared_ptr<arrow::Table> table;
    arrow::Status st = getTableFromFile(filename, table);
    if (!st.ok()) {
        std::cerr << "Error reading userstatus.parquet: " << st.ToString() << std::endl;
        return false;
    }

    // Tìm dòng cần update
    int64_t rowToUpdate = -1;
    auto userCol = table->GetColumnByName("User");
    if (!userCol) {
        std::cerr << "Column 'User' not found!" << std::endl;
        return false;
    }
    auto arr = std::static_pointer_cast<arrow::StringArray>(userCol->chunk(0));
    for (int64_t i = 0; i < table->num_rows(); ++i) {
        if (arr->GetString(i) == userName) {
            rowToUpdate = i;
            break;
        }
    }
    if (rowToUpdate == -1) {
        std::cerr << "User not found in userstatus.parquet!" << std::endl;
        return false;
    }

    // Tạo lại các cột, cập nhật các trường cần thiết
    std::vector<std::shared_ptr<arrow::Array>> newColumns;
    for (int colIdx = 0; colIdx < table->num_columns(); ++colIdx) {
        auto column = table->column(colIdx);
        auto fieldName = table->field(colIdx)->name();
        std::shared_ptr<arrow::Array> newColumn;
        auto it = status_update.find(fieldName);
        if (it != status_update.end()) {
            // Chỉ hỗ trợ string cho mọi trường
            arrow::StringBuilder builder;
            auto arr = std::static_pointer_cast<arrow::StringArray>(column->chunk(0));
            for (int64_t i = 0; i < arr->length(); ++i) {
                if (i == rowToUpdate)
                    builder.Append(it->second);
                else
                    builder.Append(arr->GetString(i));
            }
            builder.Finish(&newColumn);
        } else {
            newColumn = column->chunk(0);
        }
        newColumns.push_back(newColumn);
    }

    // Ghi lại file
    auto newTable = arrow::Table::Make(table->schema(), newColumns);
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    arrow::Status outst = arrow::io::FileOutputStream::Open(filename).Value(&outfile);
    if (!outst.ok()) {
        std::cerr << "Error opening output file: " << outst.ToString() << std::endl;
        return false;
    }
    parquet::WriterProperties::Builder props_builder;
    std::shared_ptr<parquet::WriterProperties> props = props_builder.build();
    arrow::Status write_st = parquet::arrow::WriteTable(*newTable, arrow::default_memory_pool(), outfile, 1024, props);
    if (!write_st.ok()) {
        std::cerr << "Error writing table: " << write_st.ToString() << std::endl;
        return false;
    }
    outfile->Close();
    return true;
}
//Hàm update trạng thái User Status
// bool updateUserStatus(std::string& userName, std::function<void(UserStatus&)>& updateFunc) {
        
//     return true;
// }

// hàm lấy bảng từ file user.parquet
arrow::Status getTableFromFile(const std::string& filename, std::shared_ptr<arrow::Table>& existing_table) {
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

//Hàm thêm một dòng mới vào file user.parquet
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
                                        ) {
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

//hàm chuyển đổi vector chuỗi sang vector int64_t
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

//hàm đọc file CSV và trả về bảng dữ liệu
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

//hàm chuyển đổi bảng dữ liệu từ dạng hàng sang dạng cột (transpose)
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

//hàm lưu người dùng từ file CSV vào cơ sở dữ liệu
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

// Hàm cắt chuỗi nếu quá dài
std::string TruncateString(const std::string& s, size_t max_len = 15) {
    if (s.length() <= max_len) return s;
    return s.substr(0, max_len) + "...";
}

//hàm cắt chuỗi và loại bỏ khoảng trắng ở đầu và cuối
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    return str.substr(first, last - first + 1);
}

//hàm in ra bảng dữ liệu giống như giao diện dòng lệnh
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

//hàm kiểm tra xem người dùng có tồn tại trong cơ sở dữ liệu hay không
arrow::Status printUserInfoFromDb() {
    std::string filename = "../assets/users.parquet";
    std::shared_ptr<arrow::Table> table;
    ARROW_RETURN_NOT_OK(getTableFromFile(filename, table));

    std::vector<int> column_orders = {0,1,2};
    PrintTableLikeCLI(table, column_orders);
    return arrow::Status::OK();
}

//hàm ghi log các lần đăng nhập thất bại
void logFailedLogin(std::string& userName) {
    std::ofstream logFile("login_failures.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << "Failed login attempt: " << userName << ", failed to login 3 times. Account temporarily locked." << std::endl;
        logFile.close();
    } else {
        std::cerr << "Unable to open log file." << std::endl;
    }
}

// hàm đăng nhập người dùng
void loginUser(std::shared_ptr<arrow::io::ReadableFile> infile, User *& currentUser){
    //system("clear");
    if (currentUser != nullptr) {
        std::cout << "You are already logged in as: " << currentUser << std::endl;
        UserLoginMenu(currentUser);
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
    std::string userName;
    std::cout << "User name (or 'z' to return Menu): ";
    std::getline (std::cin, userName);
    std::cin.clear(); // Clear any error state
    userName = trim(userName);
    if (userName == "z" || userName == "Z") {
        std::cout << "Returning to Menu..." << std::endl;
        currentUser = nullptr;
        return;
    }

    if (!isUserExist(userName)) {
        std::cout << "Login failed! (User invalid)" << std::endl;
        std::cout << "Press ENTER key to return Menu.....";
        std::cin.get();
        currentUser = nullptr;
        return;
    }
    
    try {
        PARQUET_ASSIGN_OR_THROW (infile, arrow::io::ReadableFile::Open("../assets/users.parquet"));
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
        //std::cout << dbUserName << std::endl;
        if (userName == dbUserName) {
            userfound = true; 
            break; 
        }
    }
    if (!userfound) {
        std::cout << "Login failed! (User invalid)" << std::endl;
        std::cout << "Press ENTER key to return Menu.....";
        std::cin.get();
        currentUser = nullptr;
        return;
    }
    std::cout << "User found: " << dbUserName << std::endl;
    //std::cin.get();
    
    UserStatus status(dbUserName, "false", "false", "false", "N/A");
    readUserStatusFromFile(dbUserName, status);
    // bool isDeleted = (status.getDeleteUser() == "true");
    // bool isLocked = (status.getFailedLogin() == "true");
    // bool isGeneratedPassword = (status.getIsGeneratedPassword() == "true");
    //bool isGeneratedPassword = (status.getIsGeneratedPassword() == "true");
    /*std::cout << "DEBUG: isGeneratedPassword=" << status.getIsGeneratedPassword()
                << ", failedLogin=" << status.getFailedLogin()
                << ", deleteUser=" << status.getDeleteUser() << std::endl; */
    // Kiểm tra trạng thái người dùng
    UserStatusType userStatus = status.checkStatus(status);
        bool canLogin = true;
        if (userStatus == UserStatusType::DELETED) {
            std::cout << "Your account has been deleted. Please contact Admin for support!" << std::endl;
            //currentUser = nullptr;
            //return;
            canLogin = false;
        } else if (userStatus == UserStatusType::LOCKED) {
            std::cout << "Your account is locked due to multiple failed login attempts. Please contact Admin for support!" << std::endl;
            //currentUser = nullptr;
            //return;
            canLogin = false;
        }

        if(!canLogin) {
            std::cout << "You cannot login at this time." << std::endl;
            std::cout << "Press ENTER key to return Menu..." << std::endl;
            std::cin.get(); // Wait for user input
            //currentUser = nullptr;
            return;
        }
    
    int failedLoginCount = 0;
    while (failedLoginCount < 3) {
        std::string userpassword;
        std::cout << "Password (or 'z' to return Menu): ";
        std::getline (std::cin, userpassword);
        userpassword = trim(userpassword);
        if (userpassword == "z" || userpassword == "Z") {
            std::cout << "Returning to Menu..." << std::endl;
            currentUser = nullptr;
            return;
        }
        // if (isvalidPassword(userpassword) == false) {
        //     std::cout << "Login failed! (Password invalid)" << std::endl;
        //     currentUser = nullptr;
        //     return;
        // }
        std::string hashedPassword = sha256(userpassword + dbSalt);
        //std::cout << "Password: " << hashedPassword << std::endl;
        if (hashedPassword == dbhasdedPassword) {
            currentUser = new User(dbFullName, dbUserName, dbhasdedPassword, dbUserPoint, dbSalt, dbWalletId);
            //Xử lý OTP
            if(verifyOTPForUser(currentUser)){
                std::cout << "Login successful!" << std::endl;
                    if (userStatus == UserStatusType::GENERATED_PASSWORD) {
                        std::cout << "Your password is generated by system." << std::endl;
                        std::cout << "Please change your Password!" << std::endl;
                        std::string fileName = "../assets/users.parquet";
                        //buộc người dùng đổi mật khẩu nếu mật khẩu được hệ thống sinh ra
                        //changeuserinfo(fileName, currentUser, false, true, false);
                        auto updated_value = changeuserinfo(fileName, currentUser, false, true, false);
                            if(!updated_value.empty()) {
                                arrow::Status status = updateUserInfo(fileName, currentUser, updated_value);
                                if(!status.ok()){
                                std::cout << "Error updating user info: " << status.ToString() << std::endl;
                            } else {
                                std::cout << "Changes password saved successfully!" << std::endl;
                            }
                            }
                        std::cout << "Press ENTER key to return menu...." << std::endl;
                        std::cin.get();
                        break;
                    }
            }
            UserLoginMenu(currentUser);
            break;
        } else {
            failedLoginCount++;
            std::cout << "Login failed! (Password invalid)!" << std::endl;
            if (failedLoginCount == 3) {
                logFailedLogin(userName);
                std::cout << "Account temporarily locked due to multiple failed login attempts." << std::endl;
                std::cout << "Please contact Admin for support" << std::endl;
                // Cập nhật trạng thái failedLogin và locked trong userstatus.parquet
                std::string statusFile = "../assets/userstatus.parquet";
                std::map<std::string, std::string> updated_values = {{"failedLogin", "true"}};
                updateUserStatusRow(statusFile, userName, updated_values);
                
                std::cout << "Press ENTER key to return Menu..." << std::endl;
                std::cin.get(); // Wait for user input
                currentUser = nullptr;
                return;
            }
        }
    }
}

void loginAdmin(std::shared_ptr<arrow::io::ReadableFile> infile, Admin*& currentAdmin) {
    if (currentAdmin != nullptr) {
        std::cout << "You are already logged in as: " << currentAdmin << std::endl;
        return;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
    std::string userAdmin;
    std::cout << "User Admin (or 'z' to return Menu): ";
    std::getline (std::cin, userAdmin);
    std::cin.clear(); // Clear any error state
    userAdmin = trim(userAdmin);
    if (userAdmin == "z" || userAdmin == "Z") {
        std::cout << "Returning to Menu..." << std::endl;
        currentAdmin = nullptr;
        return;
    }

    if (userAdmin.empty()) {
        std::cout << "Login failed! (User is not empty)" << std::endl;
        std::cout << "Press ENTER key to return Menu.....";
        std::cin.get();
        currentAdmin = nullptr;
        return;
    }
    
    try {
        PARQUET_ASSIGN_OR_THROW (infile, arrow::io::ReadableFile::Open("../assets/admin.parquet"));
    } catch (const arrow::Status& status) {
        std::cerr << "Error opening file: " << status.ToString() << std::endl;
        currentAdmin = nullptr;
        return;        
    }

    parquet::StreamReader stream{parquet::ParquetFileReader::Open(infile)};

    std::string dbFullName;
    std::string dbUserName;
    std::string dbSalt = "";
    std::string dbWalletId;
    std::string dbhasdedPassword;
    int64_t dbUserPoint;
    bool userfound = false;
 
    while (!stream.eof() ){
        stream >> dbFullName >> dbUserName >> dbhasdedPassword >> dbSalt >> dbUserPoint >> dbWalletId >> parquet::EndRow;
        //std::cout << dbUserName << std::endl;
    
        if (userAdmin == dbUserName) {
            userfound = true; 
            break; 
        }
    }
    if (!userfound) {
        std::cout << "Login failed! (User invalid)" << std::endl;
        std::cout << "Press ENTER key to return Menu.....";
        std::cin.get();
        currentAdmin = nullptr;
        return;
    }
    std::cout << "Admin found: " << dbUserName << std::endl;

    std::string adminpassword;
        std::cout << "Password (or 'z' to return Menu): ";
        std::getline (std::cin, adminpassword);
        adminpassword = trim(adminpassword);
        if (adminpassword == "z" || adminpassword == "Z") {
            std::cout << "Returning to Menu..." << std::endl;
            currentAdmin = nullptr;
            return;
        }
        bool loginSuccess = false;
        if(!dbSalt.empty()) {
        std::string hashedPassword = sha256(adminpassword + dbSalt);
            if(hashedPassword == dbhasdedPassword) {
                loginSuccess = true;
            }
        //std::cout << "Password: " << hashedPassword << std::endl;
        } else {
            if(adminpassword == dbhasdedPassword) {
                loginSuccess = true;
            }
        }
        if(loginSuccess) {
            currentAdmin = new Admin(dbFullName, dbUserName, dbhasdedPassword, dbUserPoint, dbSalt, dbWalletId);
            //AdminLoginMenu(currentAdmin);
            std::cout << "Login successful!" << std::endl;
        } else {
            std::cout << "Login failed! (Password invalid)!" << std::endl;
            currentAdmin = nullptr;
            return;
        }
    }


//Hàm sinh mã OTP
// std::string generateOTP(const std::string& WalletId = "default") {
//     // Sử dụng WalletId để tạo mã OTP duy nhất
//     auto ns = std::chrono::system_clock::now().time_since_epoch().count();
//     std::string input = std::to_string(ns) + WalletId;
//     std::string otp;
//     size_t hash = 0;
//     for (char c : input) {
//         hash = (hash * 31 + c) % 1000000; // Giới hạn OTP trong khoảng 6 chữ số
//     }
//     for(int i = 0; i < 6; ++i) {
//         otp += std::to_string((hash + i) % 10); // Chuyển đổi thành chuỗi\
//         hash /= 10;
//     }
//     return otp;
// }

//Hàm lấy thời gian hiện tại
std::string getCurruntTime() {
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local_time);
    return std::string(buffer);
}

//Hàm sinh mã giao dịch duy nhất
std::string generateTxId() {
    // Sử dụng thời gian hiện tại và một số ngẫu nhiên để tạo mã giao dịch duy nhất
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::string txId = std::to_string(now_c) + std::to_string(rand() % 1000000);
    return sha256(txId);
}

//hàm đăng ký người dùng mới (không dùng nữa)
// arrow::Status registerUser(User *& user) {
//     std::string filename = "../assets/users.parquet";
//     // Auto set user point = 0 if register
//     //user->setPoint(0);
//     std::string fullName = user->fullName();
//     std::string accountName = user->accountName();
//     std::string password = user->password();
//     std::string salt = user->salt();
//     int point = user->point();
//     std::string wallet = user->wallet();

//     arrow::Status resultRegisterUser = AppendUserParquetRow(filename,
//                                                             fullName,
//                                                             accountName,
//                                                             password,
//                                                             salt,
//                                                             point,
//                                                             wallet);
    
//     if (!resultRegisterUser.ok()) {
//         std::cerr << "Error registering user: " << resultRegisterUser.ToString() << std::endl;
//         return resultRegisterUser;
//     } 
//     std::cout << "User registered successfully!" << std::endl;
//     return arrow::Status::OK();
// }

arrow::Status findUserrow(const std::shared_ptr<arrow::Table>& table, 
                          const std::string& userName,
                          const std::string& walletId,
                          int64_t& rowToUpdate) {
                            
    auto userNameColumn = table->GetColumnByName("UserName");
    auto walletIdColumn = table->GetColumnByName("IDWallet");
    if(!userNameColumn || !walletIdColumn) {
        std::cerr << "UserName or IDWallet column not found" << std::endl;
        return arrow::Status::Invalid("UserName or IDWallet column not found");
    }
    if(userNameColumn->type() -> id() != arrow::Type::STRING || walletIdColumn->type() -> id() != arrow::Type::STRING) {
        std::cerr << "UserName or IDWallet column  is not a string" << std::endl;
        return arrow::Status::Invalid("UserName or IDWallet column  is not a string");
    }

    int64_t globalRow = 0;
    for(int chunkIdx = 0; chunkIdx < userNameColumn->num_chunks(); ++chunkIdx) {
        auto userNamearray = std::static_pointer_cast<arrow::StringArray>(userNameColumn->chunk(chunkIdx));
        auto walletIdArray = std::static_pointer_cast<arrow::StringArray>(walletIdColumn->chunk(chunkIdx));
        for(int64_t i = 0; i < userNamearray->length(); i++, globalRow++) {
            if(userNamearray->IsNull(i) || walletIdArray->IsNull(i)) {
                continue; // Skip null values
            }
            if(userNamearray->GetString(i) == userName && walletIdArray->GetString(i) == walletId) {
                rowToUpdate = globalRow;
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
                             std::map<std::string, 
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
    if(updated_values.find("IDWallet") != updated_values.end()) {
        std::cerr << "WalletId cannot be update!" << std::endl;
        return arrow::Status::Invalid("WalletId cannot be update!");
    }
    // if(!allow_point_update && updated_values.find("Points") != updated_values.end()) {
    //     std::cerr << "Points cannot be update!" << std::endl;
    //     return arrow::Status::Invalid("Points cannot be update!");
    // }

    std::cout << "Starting updateUserInfo for file: " << filename << ", user: " << user->accountName() << ", Wallet: " << user->wallet() << std::endl;
    
    // Đọc bảng từ file parquet
    std::shared_ptr<arrow::Table> table;
    ARROW_RETURN_NOT_OK(getTableFromFile(const_cast<std::string&>(filename), table));
        
    std::cout << "Table read successfully, num_rows: " << table->num_rows() << ", num_columns: " << table->num_columns() << std::endl;

    // Kiểm tra các cột cần cập nhật
    std::set<std::string> requires_columns = {"UserName", "IDWallet"};
    std::map<std::string, arrow::Type::type> expected_types = {
        {"UserName", arrow::Type::STRING},
        {"FullName", arrow::Type::STRING},
        {"UserPassword", arrow::Type::STRING},
        {"Salt", arrow::Type::STRING},
        {"Points", arrow::Type::INT64},
        {"IDWallet", arrow::Type::STRING}
    };
    for(const auto& [col, _] : updated_values) {
        requires_columns.insert(col);
    }
    ARROW_RETURN_NOT_OK(checkRequiredColumns(table, requires_columns, expected_types));
    // Tìm hàng cần cập nhật
    int64_t rowToUpdate = -1;
    if(rowToUpdate == -1) {
    ARROW_RETURN_NOT_OK(findUserrow(table, user->accountName(), user->wallet(), rowToUpdate));
    }
    std::cout << "Found user at row: " << rowToUpdate << std::endl;

    //Kiểm tra WalletId
    auto walletIdColumn = table->GetColumnByName("IDWallet");
    if(!walletIdColumn) {
        std::cerr << "Column WalletId not found" << std::endl;
        return arrow::Status::Invalid("Column WalletId not found");
    }
    auto walletIDarray = std::static_pointer_cast<arrow::StringArray>(walletIdColumn->chunk(0));
    if(walletIDarray->GetString(rowToUpdate) != user->wallet()) {
        std::cerr << "WalletId does not match with user" << std::endl;
        return arrow::Status::Invalid("WalletId does not match with user");
    }

    //Tạo cột mới
    std::vector<std::shared_ptr<arrow::Array>> newColumns;
    for(int colIdx = 0; colIdx < table->num_columns(); ++colIdx) {
        auto column = table->column(colIdx);
        auto fieldName = table->field(colIdx)->name();
        std::shared_ptr<arrow::Array> newColumn;
        //Nếu cột cần cập nhật
        auto it = updated_values.find(fieldName);
        if(it != updated_values.end()) {
            if(fieldName == "Points") {
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
                                                    10240,
                                                    props));
        ARROW_RETURN_NOT_OK(outfile->Close());
        std::cout << "Successfully updated user info in file: " << filename << std::endl;
        return arrow::Status::OK();
}

//Hàm ghi log giao dịch
void logTransaction(const std::string& senderWalletId,
                    const std::string& senderuserName,
                    const std::string& senderFullName, 
                    const std::string& receiverWalletId,
                    const std::string& receiveruserName,
                    const std::string& receiverFullName, 
                    int64_t transferPoint,
                    bool isSuccess,
                    const std::string& errorMessage) {
    const std::string logFilename = "../logs/transaction.log"; // Đường dẫn đến file log 
    // Kiểm tra kích thước file log, nếu lớn hơn 100MB thì backup và tạo file mới
    std::filesystem::path logPath(logFilename); // Chuyển đổi đường dẫn thành đối tượng std::filesystem::path
    if(!std::filesystem::exists(logPath.parent_path())) { // Kiểm tra xem thư mục chứa file log có tồn tại không
        // Nếu không tồn tại, tạo thư mục
        std::filesystem::create_directories(logPath.parent_path());
    }
    // Giới hạn kích thước file log là 100MB
    // Nếu file log đã tồn tại, kiểm tra kích thước của nó
    // Nếu kích thước lớn hơn 100MB, backup file log và tạo file mới
    // Kiểm tra xem file log đã tồn tại hay chưa
    if(!std::filesystem::exists(logFilename)) {
        // Nếu file log chưa tồn tại, tạo mới
        std::ofstream logFile(logFilename);
        if(!logFile.is_open()) {
            std::cerr << "Error: Unable to create log file!" << std::endl;
            return;
        }
        logFile.close();
    }
    
    const uint64_t maxfileSize = 100 * 1024 * 1024; // 100MB

    if(std::filesystem::exists(logFilename)) {
        uint64_t fileSize = std::filesystem::file_size(logFilename);
        if(fileSize > maxfileSize) {
            std::string backupFilename = "...logs/transaction_" + getCurruntTime().substr(0, 10) + ".log";
            std::rename(logFilename.c_str(), backupFilename.c_str());
        }

        //std::string generate_TxId = generateTxId();
        //auto timestamp = std::chrono::system_clock::now();
        //auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()).count();
        // Ghi log vào file
        std::ofstream logFile(logFilename, std::ios::app);
        if(logFile.is_open()) {
            logFile << "[" << generateTxId() << " " << getCurruntTime() << "] Transfer"
                    << " From WalletId = " << senderWalletId << " (" << senderuserName << ", " << senderFullName << ")"
                    << " To WalletId = " << receiverWalletId << " (" << receiveruserName << ", " << receiverFullName << ")"
                    << " Points transferred: " << transferPoint << " Status: " << (isSuccess ? "Success" : "Failed") << "\n";
            logFile.close();
        } else {
            std::cerr << "Error: Unable to open log file!" << std::endl;
        }
        // if(file.tellg() > maxfileSize) {
        //     file.close();
        //     std::remove("transaction.log");
        }
}

bool walletIdExists = false;
//Hàm kiểm tra WalletId và FFullName
bool checkWalletIdAndFullName(const std::string& filename,
                              const std::string& walletId, 
                              const std::string& fullName,
                              int64_t& receiverPoints,
                              int& receiverRow,
                              std::string& receiverUserName,
                              std::string& errorMessage) {
    std::cout << "Checking WalletId: " << walletId << ", Fullname" << fullName << std::endl;
    // Đọc file parquet
    std::shared_ptr<arrow::Table> table;
    arrow::Status status = getTableFromFile(filename, table);
    if(!status.ok()) {
        errorMessage = "Error reading file: " + status.ToString();
        std::cerr << errorMessage << std::endl;
        return false;
    }

    if(!table || table->num_rows() == 0) {
        errorMessage = "Table is empty or not found!";
        std::cerr << errorMessage << std::endl;
        return false;
    }

    auto walletIdColumn = table->GetColumnByName("IDWallet");
    auto fullNameColumn = table->GetColumnByName("Fullname");
    auto userNameColumn = table->GetColumnByName("UserName");
    auto pointColumn = table->GetColumnByName("Points");
    if(!walletIdColumn || !fullNameColumn || !userNameColumn || !pointColumn) {
        errorMessage = "Required columns (IDWallet, Fullname, UserName, Point) not found!";
        std::cerr << errorMessage << std::endl;
        return false;
    }

    // Kiểm tra kiểu dữ liệu của các cột
    if(walletIdColumn->type()->id() != arrow::Type::STRING ||
       fullNameColumn->type()->id() != arrow::Type::STRING ||
       userNameColumn->type()->id() != arrow::Type::STRING ||
       pointColumn->type()->id() != arrow::Type::INT64) {
        errorMessage = "Columns have types mismatch (expected STRING for WalletId and Fullname, INT64 for Point)!";
        std::cerr << errorMessage << std::endl;
        return false;
    }

    // Kiểm tra WalletId và FullName
    int64_t globalRowCount = 0;    
    bool walletIdFound = false;
    for (int chunkIdx = 0; chunkIdx < walletIdColumn->num_chunks(); ++chunkIdx) {
        auto walletIdArray = std::static_pointer_cast<arrow::StringArray>(walletIdColumn->chunk(chunkIdx));
        auto fullNameArray = std::static_pointer_cast<arrow::StringArray>(fullNameColumn->chunk(chunkIdx));
        auto userNameArray = std::static_pointer_cast<arrow::StringArray>(userNameColumn->chunk(chunkIdx));
        auto pointArray = std::static_pointer_cast<arrow::Int64Array>(pointColumn->chunk(chunkIdx));

        std::cout << "Processing chunk " << chunkIdx << " with " << walletIdArray->length() << " row" << std::endl;

        for (int64_t i = 0; i < walletIdArray->length(); ++i, ++globalRowCount) {
            if (walletIdArray->IsNull(i) || fullNameArray->IsNull(i)) {
                continue; // Bỏ qua các giá trị null
            }
            if (walletIdArray->GetString(i) == walletId && fullNameArray->GetString(i) == fullName) {
                if (pointArray->IsNull(i) || userNameArray->IsNull(i)) {
                    errorMessage = "Point or UserName is null for the given WalletId and FullName!";
                    std::cerr << errorMessage << std::endl;
                    return false;
                }
                receiverRow = globalRowCount;
                receiverPoints = pointArray->Value(i);
                receiverUserName = userNameArray->GetString(i);
                walletIdExists = true;
                std:: cout << "Found receiver at global row: " << receiverRow << ", UserName: " << receiverUserName << std::endl;
                return true; // Tìm thấy WalletId và FullName
            }
        }
    }

    errorMessage = "WalletId or FullName not found!";
    std::cerr << errorMessage << std::endl;
    return false; // Không tìm thấy WalletId và FullName
}

// Hàm chuyển điểm
arrow::Status transferPoint(const std::string& filename, User *& currentUser) {
    std::cout << "Starting transferPoint with file: " << filename << std::endl;
    // Kiểm tra đầu vào
    if(!currentUser) {
        std::cerr << "Error: User is null!" << std::endl;
        return arrow::Status::Invalid("User is null");
    }

    if(!std::filesystem::exists(filename)) {
        std::cerr << "Error: File does not exist!" << std::endl;
        return arrow::Status::Invalid("File does not exist");
    }
    
    std::string receiverWalletId;
    std::string receiverFullName;
    std::string receiverUserName;
    int64_t receiverPoints;
    int receiverRow;
    int64_t transferPoint = 0;
    
    while (true) {
        std::cout << "Enter receiver's wallet ID (or 'z' to return Menu): ";
        std::getline(std::cin, receiverWalletId);
        receiverWalletId = trim(receiverWalletId);
        if (receiverWalletId == "z" || receiverWalletId == "Z") {
            std::cout << "Returning to Menu." << std::endl;
            return arrow::Status::OK();
        }
        if (receiverWalletId.empty()) {
            std::cout << "Invalid wallet ID. Please try again." << std::endl;
            continue;
        }
        std::cout << "Enter receiver's full name (or 'z' to return Menu): ";
        std::getline(std::cin, receiverFullName);
        receiverFullName = trim(receiverFullName);
        if (receiverFullName == "z" || receiverFullName == "Z") {
            std::cout << "Returning to Menu." << std::endl;
            return arrow::Status::OK();
        }
        if (receiverFullName.empty()) {
            std::cout << "Invalid full name. Please try again." << std::endl;
            continue;
        } else if (receiverWalletId == currentUser->wallet()) {
            std::cout << "You cannot transfer points to yourself." << std::endl;
            continue;
        }
        
        std::cout << "Enter points to transfer: ";
        std::cin >> transferPoint;
        if (std::cin.fail() || transferPoint <= 0) {
            std::cin.clear(); // Xóa trạng thái lỗi
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Bỏ qua dòng nhập không hợp lệ
            std::cout << "Invalid transfer points. Please enter a valid number again!" << std::endl;
            continue;
        }
        
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Bỏ qua ký tự newline còn lại trong buffer

        // Kiểm tra xem người dùng có đủ điểm để chuyển không
        if (transferPoint > currentUser->point()) {
            std::cout << "You do not have enough points to transfer." << std::endl;
            continue;
        }

        std::string errorMessage;
        if(!checkWalletIdAndFullName(filename, 
                                     receiverWalletId, 
                                     receiverFullName, 
                                     receiverPoints, 
                                     receiverRow, 
                                     receiverUserName, 
                                     errorMessage)) {
            std::cout << "Error: " << errorMessage << std::endl;
            continue;
        }

        // Xác nhận giao dịch
        std::string confirm;
        std::cout << "Are you sure you want to transfer " << transferPoint << " points to " << receiverFullName << "? (Yes/No): ";
        std::getline(std::cin, confirm);
        if (confirm != "Y" && confirm != "y") {
            std::cout << "Transaction cancelled." << std::endl;
            logTransaction(currentUser->wallet(), 
                            currentUser->accountName(), 
                            currentUser->fullName(), 
                            receiverWalletId, 
                            receiverUserName, 
                            receiverFullName, 
                            transferPoint, false);
            return arrow::Status::OK();
        }

        //Xử lý OTP
        int otpAttempts = 0;
        const int maxOtpAttempts = 3;
        bool otpVerified = false;
        std::string otp, userOtp;

        while (otpAttempts < maxOtpAttempts) {
            otp = generateOTP(currentUser->wallet(), currentUser->accountName());
            std::cout << "Your OTP is: " << otp << std::endl;
            std::cout << "Enter the OTP: ";
            getline(std::cin, userOtp);
            userOtp = trim(userOtp);
            if (userOtp == otp) {
                otpVerified = true;
                break;
            } else {
                std::cout << "Invalid OTP. Please try again." << std::endl;
                otpAttempts++;
            }
        }

        if(!verifyOTP(userOtp, currentUser->wallet(), currentUser->accountName())) {
            std::cout << "You entered incorrect OTP 3 time. Transaction cancelled." << std::endl;
            logTransaction(currentUser->wallet(), currentUser->accountName(), currentUser->fullName(), receiverWalletId, receiverUserName, receiverFullName, transferPoint, false);
            return arrow::Status::Invalid("Invalid OTP");
        }

        
        // câp nhật điểm cho người gửi
        std::map<std::string, std::string> senderUpdatedValues = {
            {"Points", std::to_string(currentUser->point() - transferPoint)}
        };
        arrow::Status status = updateUserInfo(filename, currentUser, senderUpdatedValues, true);
        if (!status.ok()) {
            std::cerr << "Error updating sender's points: " << status.ToString() << std::endl;
            logTransaction(currentUser->wallet(), 
                            currentUser->accountName(), 
                            currentUser->fullName(), 
                            receiverWalletId, 
                            receiverUserName, 
                            receiverFullName, 
                            transferPoint, false);
            return arrow::Status::IOError("Failed to update sender's points");
        }

        // Create json message for producer
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%d-%m-%Y %H:%M:%S");
        std::map<std::string, std::string> transfer_msg = {
            {"Sender", currentUser->wallet()},
            {"Time", ss.str()},
            {"Point", std::to_string(transferPoint)}
        };

        // Convert map to json
        std::string transfer_msg_json = map_to_json(transfer_msg);
        // Send from producer to broker
        currentUser->sendMessageToKafka(transfer_msg_json, receiverWalletId);

        // câp nhật điểm cho người nhận
        User receiver(receiverFullName, receiverUserName, "", receiverPoints, "", receiverWalletId);
        std::map<std::string, std::string> receiverUpdatedValues = {
            {"Points", std::to_string(receiverPoints + transferPoint)}
        };
        User* receiverPtr = &receiver;
        status = updateUserInfo(filename, receiverPtr, receiverUpdatedValues, true);
        if (!status.ok()) {
            std::cerr << "Error updating receiver's points: " << status.ToString() << std::endl;
            logTransaction(currentUser->wallet(), 
                            currentUser->accountName(), 
                            currentUser->fullName(), 
                            receiverWalletId, 
                            receiverUserName, 
                            receiverFullName, 
                            transferPoint, false);
            // std::remove(tempFilename.c_str());
            return arrow::Status::IOError("Failed to update receiver's points");
        }

        //cập nhật điểm trong đối tượng currentUser
        currentUser->setPoint(currentUser->point() - transferPoint);
        // Ghi log giao dịch
        std::cout << "Transaction successful! " << transferPoint << " points transferred to " << receiverFullName << "." << std::endl;
        logTransaction(currentUser->wallet(), 
                        currentUser->accountName(), 
                        currentUser->fullName(), 
                        receiverWalletId, 
                        receiverUserName, 
                        receiverFullName, 
                        transferPoint, true);
        break;
    }
    return arrow::Status::OK();
}
    