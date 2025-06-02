#include <iostream>
#include "User.h"
#include "Admin.h"
#include <string>
#include "arrow/io/file.h"
#include "parquet/stream_reader.h"
#include "parquet/arrow/writer.h"
#include <openssl/sha.h>
#include "DbUtils.h"
#include "Menus.h"
using namespace std;

void printTestKafKaMenu() {

}
void menu() {
    User user1;
    
    cout << "1. Send message" << endl;
    cout << "2. Receive message" << endl;
    cout << "Enter your choice: ";
    int choice;
    cin >> choice;
    if (choice == 1)  {
        string topic = "test";
        string message = "Test from producer";
        while (true)
        {
            getline(cin, message);
            user1.sendMessageToKafka(message, topic);
            // cin.ignore();

        }
        
        
    } else {
        string topic = "test";
        while (true) {
            cout << "Test thread" << endl;
        }
    }
}

int main () {
    Client * currentClient = new Client();
    while (true)
    {
        system("cls");
        mainMenu(currentClient);
    }
    

    return 0;
}
