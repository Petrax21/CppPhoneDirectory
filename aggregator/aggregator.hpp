#ifndef AGGREGATOR_HPP
#define AGGREGATOR_HPP

#include <string>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <nlohmann/json.hpp>

class Aggregator {
public:
    Aggregator(const std::string& rabbitmqAddress, const std::string& queueName);

    [[noreturn]] void startListening();

private:
    std::string queueName;
    AMQP::LibEvHandler handler;
    AMQP::TcpConnection connection;
    AMQP::TcpChannel channel;

    // Mesajları işler
    void processMessage(const AMQP::Message &message, uint64_t deliveryTag);
};

#endif AGGREGATOR_HPP
