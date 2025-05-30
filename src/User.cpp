#include "User.h"
#include <string>
#include <iostream>
#include <librdkafka/rdkafka.h>

void User::initKafkaClient() {
    std::string brokers = "100.116.29.42:9092";
    char errstr[512];

    this->producer_conf = rd_kafka_conf_new();
    
    // Init config cho producer
    if(rd_kafka_conf_set(this->producer_conf, "bootstrap.servers", brokers.c_str(), errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
        std::cerr << "Error setting brokers: " << errstr << std::endl;
    }
    
    this->consumer_conf = rd_kafka_conf_new();
    // Init config cho consumer
    if (rd_kafka_conf_set(this->consumer_conf, "group.id", this->group_id.c_str(), errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
        std::cerr << "Error setting group.id: " << errstr << std::endl;
        return;
    }

    rd_kafka_conf_set(this->consumer_conf, "enable.auto.commit", "true", nullptr, 0);

    // Tạo producer
    this->producer = rd_kafka_new(RD_KAFKA_PRODUCER, this->producer_conf, errstr, sizeof(errstr));
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        return;
    }
    // Tạo consumer
    this->consumer = rd_kafka_new(RD_KAFKA_CONSUMER, this->consumer_conf, errstr, sizeof(errstr));
    if (!consumer) {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        return;
    }

}

User::User(){
    initKafkaClient();
}

User::User(std::string FullNameArg, std::string AccountNameArg, std::string PasswordArg, int PointArg, std::string SaltArg, std::string WalletArg)
{

    this->FullName = FullNameArg;
    this->AccountName = AccountNameArg;
    this->Password = PasswordArg;
    this->Point = PointArg;
    this->Salt = SaltArg;
    this->Wallet = WalletArg;

}



std::string User::salt()  {
    return this->Salt;
}
void User::setSalt( std::string &salt) {
    this->Salt = salt;
}

std::string User::fullName()  {
    return this->FullName;
}
void User::setFullName( std::string &FullName) {
    this->FullName = FullName;
}

std::string User::accountName()  {
    return this->AccountName;
}
void User::setAccountName( std::string &accountName) {
    this->AccountName = accountName;
}

std::string User::password()  {
    return this->Password;
}
void User::setPassword( std::string &password) {
    this->Password = password;
}

int User::point()  {
    return this->Point;
}
void User::setPoint(int point) {
    this->Point = point;
}

std::string User::wallet()  {
    return this->Wallet;
}
void User::setWallet(std::string &wallet) {
    this->Wallet = wallet;
}

void User::sendMessageToKafka(std::string message, std::string topic) {
    if (rd_kafka_producev(
        this->producer,
        RD_KAFKA_V_TOPIC(topic.c_str()),
        RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
        RD_KAFKA_V_VALUE(const_cast<char*>(message.c_str()), message.size()),
        RD_KAFKA_V_END
    ) != 0) {
        std::cerr << "Failed to produce: " << rd_kafka_err2str(rd_kafka_last_error()) << std::endl;
    } else {
        std::cout << "Produced message: " << message << std::endl;
    }

    rd_kafka_flush(this->producer, 5000);
    rd_kafka_destroy(this->producer);

};

void User::receiveMessageFromKafka(std::string topic) {
    // Đăng ký topic
    rd_kafka_poll_set_consumer(this->consumer);
    rd_kafka_topic_partition_list_t *topics = rd_kafka_topic_partition_list_new(1);
    rd_kafka_topic_partition_list_add(topics, topic.c_str(), -1);   
    if (rd_kafka_subscribe(this->consumer, topics) != RD_KAFKA_RESP_ERR_NO_ERROR) {
        std::cerr << "Failed to subscribe to topic\n";
        rd_kafka_destroy(this->consumer);
        return;
    }

    std::cout << "Consumer started, waiting for message ..." << std::endl;
    while(true) {
        rd_kafka_message_t* msg = rd_kafka_consumer_poll(this->consumer, 1000);
        if (!msg) continue;

        if (msg->err) {
            if (msg->err == RD_KAFKA_RESP_ERR__PARTITION_EOF) {
                // Đã đến cuối partition
            } else {
                std::cerr << "Consumer error: " << rd_kafka_message_errstr(msg) << std::endl;
            }
        } else {
            std::string payload((char*)msg->payload, msg->len);
            std::cout << "Received message: " << payload << std::endl;
        }
        rd_kafka_message_destroy(msg);
    }
    
    // rd_kafka_consumer_close(this->consumer);
    // rd_kafka_destroy(this->consumer);

}

User::~User()
{
}
