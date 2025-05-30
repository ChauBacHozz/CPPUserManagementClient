#include <iostream>
#include "User.h"
#include "Admin.h"
#include <string>
#include "arrow/io/file.h"
#include "parquet/stream_reader.h"
#include <openssl/sha.h>
#include "DbUtils.h"
#include "Menus.h"
using namespace std;


int main () {
    while (true)
    {
        system("cls");
        mainMenu();
    }
    




    return 0;
}