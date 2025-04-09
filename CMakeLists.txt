# Set minimum CMake version and project
cmake_minimum_required(VERSION 3.10)
project(DS_Alg)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include and lib directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib
    "C:/Program Files (x86)/arrow/include"  # Arrow headers (adjust if needed)
)

link_directories(
    "C:/Program Files (x86)/arrow/lib"      # Arrow .a and .dll.a files
)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "lib/**/*.cpp")

# Output directory
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)

# Build executable
add_executable(main ${SOURCES})

# Link libraries (no .lib/.a extension needed)
target_link_libraries(main
    arrow parquet
)
