#pragma once
#include <string>
#include <librdkafka/rdkafka.h>
#include <thread>
#include "Client.h"

class User : public Client
{
private:
    std::string FullName;
    std::string AccountName;
    std::string Password;
    std::string Salt;
    std::string Wallet;
    int64_t Point = 0;
    // Kafa client init;
    std::string broker = "100.116.29.42:9092";
    rd_kafka_conf_t* producer_conf;
    rd_kafka_conf_t* consumer_conf;
    std::string group_id = "consumer_group";
    rd_kafka_t* producer;
    rd_kafka_t* consumer;
    // Kafka consumer thread
    std::thread consumer_thread;
    bool consumer_thread_running = false;
    
    
public:
    User();
    User(std::string FullNameArg, std::string AccountNameArg, 
         std::string PasswordArg, int64_t PointArg, std::string SaltArg, 
         std::string WalletArg);
    ~User();

    void initKafkaClient();
    void activateConsumerThread();
    bool check_consumer_thread_running();
    


    std::string fullName() ;
    void setFullName( std::string &fullName);

    std::string accountName() ;
    void setAccountName( std::string &accountName);


    std::string password() ;
    void setPassword( std::string &password);


    int64_t point() ;
    void setPoint(int64_t point);

    std::string salt() ;
    void setSalt( std::string &salt);

    std::string wallet() ;
    void setWallet( std::string &wallet);

    void sendMessageToKafka(std::string message, std::string topic);
    void receiveMessageFromKafka(std::string topic);

};

