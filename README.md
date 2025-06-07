# Giới thiệu dự án CppPtitProjectPrototype

CppPtitProjectPrototype là một dự án C++ được thiết kế để quản lý thông tin người dùng. Nó được sử dụng hỗ trợ cho cả người dùng bình thường và quản trị viên, bao gồm các chức năng như:
- Tạo và quản lý thông tin tài khoản: có thông tin cá nhân, mã hóa thông tin bảo mật.
- Quản lý điểm và giao dịch.
- Lưu trữ thông tin dữ liệu bằng Apache Arrow và Parquet.

##  Chức năng

-  ** Quản lý thông tin tài khoản
  - Tạo và quản lý thông tin tài khoản, với mật khẩu được mã hóa riêng.
  - Mỗi tài khoản có các thông tin như tên người dùng, tài khoản, mật khẩu, mã ví mã hóa và số dư điểm.


-  ** Quản trị viên
  - Quản trị viên có thể xem và chỉnh sửa thông tin người dùng (các mục cho phép).
  - Chỉ tài khoản quản trị viên có thể đổi thông tin của mình đảm bảo bảo mật.
  - Quản trị viên có thể tác động và kiểm tra số dư trên ví hệ thống.

-  ** Quản lý dữ liệu
  - Thông tin tài khoản của người dùng và quản trị viên được lưu trong file Parquet, sử dụng thư viện Apache Arrow.
  - Dữ liệu được cập nhật bằng API.

- ** Bảo mật
  - Mật khẩu được xử lý bằng thuật toán SHA-256 với một chuỗi riêng.
  - Chuỗi riêng và mật khẩu được lưu trữ đảm bảo an toàn và bảo mật riêng.

##  Các loại ứng dụng được sử dụng

- **C++17**
- **CMake** (build system)
- **Apache Arrow** and **Parquet** (tuần tự hóa và lưu trữ dữ liệu)
- **OpenSSL** (SHA-256 hashing)
- **librdkafka** (Kafka client library)
- **Standard Template Library (STL)**

##  Cấu trúc dự án

```
CppPtitProjectPrototype/
├── assets/
│   ├── admin.parquet
│   ├── user_info.csv
│   ├── users.parquet
│   ├── userstatus.parquet
├── include/          # Header files
│   ├── arrow/
│   ├── ├──...
│   ├── librdkafka/
│   ├── ├──...
│   ├── openssl/
│   ├── ├──...
│   ├── parquet/
│   ├── ├──...
│   ├── Admin.h
│   ├── Client.h
│   ├── DbUtils.h
│   ├── Encrypt.h
│   ├── JsonUtils.h
│   ├── Menus.h
│   ├── transaction_utils.h #OTP
│   ├── User.h
│   ├── UserStatus.h  
├── lib/              # External libraries (.dll, .lib, .a files)
├── log/
│   ├── OTP.log
│   ├── editspoint.log
│   ├── transaction.log
│   ├── transaction.log.txt
├── src/
│   ├── main.cpp
│   ├── Admin.cpp
│   ├── Client.cpp
│   ├── DbUtils.cpp
│   ├── Encrypt.cpp
│   ├── JsonUtils.cpp
│   ├── Menus.cpp
│   ├── OTP.cpp
│   ├── User.cpp
│   ├── UserStatus.cpp  
├── CMakeLists.txt
└── README.md
└── run.bat
```

##  Build Instructions

### Prerequisites

- **CMake** >= 3.10
- **C++17-compatible** compiler (e.g., `g++`, `clang++`, hoặc MSVC)
- **Apache Arrow** và **Parquet** thư viện phát triển
- **OpenSSL** (`libssl` và `libcrypto`)
- **librdkafka** (thư viện Kafka, tùy chọn đã được bao gồm)
- Yêu cầu các file định dạng `.dll` cần được để đúng trong thư mục `lib/`

### Tạo dự án và chạy chương trình

```bash
git clone https://github.com/ChauBacHozz/CppPtitProjectPrototype.git
cd CppPtitProjectPrototype
./run.bat
```

Tập tin được clone về, sử dụng "./run.bat" để chạy chương trình
Chương trình sẽ hiển thị hướng dẫn trên màn hình để bạn đăng nhập với vai trò user hoặc admin.

###  Chi tiết liên kết thư viện bằng CMake

Dự án này liên kết tĩnh các thư viện sau:

- `arrow`, `parquet` — dùng để lưu trữ dữ liệu dạng cột hiệu quả
- `libssl`, `libcrypto` — để mã hóa mật khẩu bằng thuật toán SHA-256
- `rdkafka`, `rdkafkacpp` — (tuỳ chọn) hỗ trợ mở rộng với Kafka

File `CMakeLists.txt` có đoạn mã tự động sao chép các file `.dll` cần thiết vào thư mục chứa file thực thi sau khi build thành công:

```
add_custom_command(TARGET main POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "lib/libarrow.dll"
    "lib/libparquet.dll"
    "lib/libcrypto-3-x64.dll"
    "lib/libssl-3-x64.dll"
    "lib/librdkafka.dll"
    "lib/librdkafkacpp.dll"
    $<TARGET_FILE_DIR:main>
)
```

- Trên Windows, bạn có thể cần điều chỉnh đường dẫn trong `CMakeLists.txt` để khớp với vị trí cài đặt thực tế của các thư viện Arrow, Parquet, OpenSSL và Kafka trên máy của bạn.

##  Example Use Cases

- Đăng nhập Admin: Nhập tài khoản admin để truy cập các tính năng quản lý.
- Đổi tên/mật khẩu Admin: Sử dụng các hàm có sẵn để cập nhật thông tin cá nhân một cách an toàn.
- Quản lý điểm số: Admin có thể chuyển điểm hoặc thay đổi điểm số cho user, đồng thời cập nhật ví.
- Lưu trữ dữ liệu lâu dài: Tất cả thay đổi đều được ghi lại trong file Parquet và vẫn tồn tại sau khi đóng chương trình.

##  Hướng phát triển trong tương lai

- Xác thực đăng nhập bằng OTP
- Giao diện dòng lệnh (CLI) hoặc giao diện đồ họa (GUI) phân quyền rõ ràng
- Kiểm tra đầu vào và xử lý lỗi tốt hơn
- Ghi log hệ thống và lịch sử thao tác

##  License

Dự án này được phát triển phục vụ mục đích học tập tại PTIT. Không sử dụng cho mục đích thương mại.