# Set minimum CMake version and project
cmake_minimum_required(VERSION 3.10)
project(DS_Alg)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include and lib directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    # "C:/Program Files (x86)/arrow/include"  # Arrow headers (adjust if needed)
    ${CMAKE_SOURCE_DIR}/lib

)
    
link_directories(
    # "C:/Program Files (x86)/arrow/lib"      # Arrow .a and .dll.a files
    ${CMAKE_SOURCE_DIR}/lib
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
    libssl
    libcrypto
)

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/lib/libarrow.dll"
        "${CMAKE_SOURCE_DIR}/lib/libparquet.dll"
        "${CMAKE_SOURCE_DIR}/lib/libcrypto-3-x64.dll"
        "${CMAKE_SOURCE_DIR}/lib/libssl-3-x64.dll"
        $<TARGET_FILE_DIR:main>)