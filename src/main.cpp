#include <iostream>
#include <string>
#include "Backend.h"
#include "User.h"
#include "Admin.h"
#include <string>
#include "arrow/io/file.h"
#include "parquet/stream_reader.h"
#include <openssl/sha.h>
#include "DbUtils.h"
#include "Menus.h"

int main() {
    Client * currentClient = new Client();
    while (true)
    {
        system("cls");
        mainMenu(currentClient);
    }
    return 0;
}
