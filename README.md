# CppPtitProjectPrototype

CppPtitProjectPrototype is a C++ project designed as a prototype for a user management system. It supports both regular users and administrators, and includes functionality for account creation, password encryption, points management, and secure data storage using Apache Arrow and Parquet.

##  Features

-  **User Account Management**
  - Create and manage user accounts with encrypted passwords.
  - Each user has attributes like full name, username, password, salt, wallet, and point balance.

-  **Admin Controls**
  - Admin accounts can view and update user data.
  - Admins can change their own name and password securely.
  - Admins can adjust points and manage wallet balances.

-  **Data Storage**
  - User and admin data is stored in Parquet files using the Apache Arrow library.
  - Efficient data updates through Arrow APIs.

-  **Security**
  - Passwords are hashed using SHA-256 with salt.
  - Passwords and salts are securely updated and stored.

##  Technologies Used

- **C++17**
- **CMake** (build system)
- **Apache Arrow** and **Parquet** (data serialization and storage)
- **OpenSSL** (SHA-256 hashing)
- **librdkafka** (Kafka client library)
- **Standard Template Library (STL)**

##  Project Structure

```
CppPtitProjectPrototype/
├── src/
│   ├── main.cpp
│   ├── Admin.cpp / Admin.h
│   ├── User.cpp / User.h
│   ├── Encrypt.h
│   ├── DbUtils.h / DbUtils.cpp
│   └── ...
├── lib/              # External libraries (.dll, .lib, .a files)
├── include/          # Header files
├── CMakeLists.txt
└── README.md
```

##  Build Instructions

### Prerequisites

- **CMake** >= 3.10
- **C++17-compatible** compiler (e.g., `g++`, `clang++`, or MSVC)
- **Apache Arrow** and **Parquet** development libraries
- **OpenSSL** (`libssl` and `libcrypto`)
- **librdkafka** (Kafka client library, optional but included)
- Required `.dll` files should be placed in the `lib/` directory or available in system PATH

### Building the Project

```bash
git clone https://github.com/ChauBacHozz/CppPtitProjectPrototype.git
cd CppPtitProjectPrototype
mkdir build && cd build
cmake ..
make
```

The compiled binary will be placed in the `build/` directory as `main`.

###  CMake Linking Details

This project statically links the following libraries:

- `arrow`, `parquet` — for columnar data storage
- `libssl`, `libcrypto` — for SHA-256 password encryption
- `rdkafka`, `rdkafkacpp` — (optional) for Kafka-based extensions

The `CMakeLists.txt` also copies necessary `.dll` files automatically after build:

```cmake
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

>  On Windows, you may need to adjust paths in `CMakeLists.txt` to match your actual installation of Arrow, Parquet, OpenSSL, and Kafka.

## ▶ Running the Program

After successful build, the executable will be located in the `build/` directory:

```bash
./main
```

Follow the on-screen prompts to log in as an admin or user.

##  Example Use Cases

- **Admin Login**: Enter admin credentials to gain access to management features.
- **Change Admin Name/Password**: Use provided functions to securely update name or password.
- **Points Management**: Admins can transfer or edit user points with wallet synchronization.
- **Persistent Storage**: All changes are stored in Parquet files and reflected in subsequent sessions.

##  Future Improvements

- OTP verification for secure login
- CLI-based or GUI-based role interactions
- Input validation and better error handling
- Logging system and audit trails

##  License

This project was developed for educational purposes at PTIT. Not intended for commercial use.