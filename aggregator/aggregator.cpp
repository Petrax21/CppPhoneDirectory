#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include "rabbit-mq-adapter.hpp"
#include "aggregator.hpp"

const std::string QUEUE_NAME = "contacts_queue";

[[noreturn]] int main() {
    struct ev_loop *loop = EV_DEFAULT;
    AMQP::LibEvHandler handler(loop);
    AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://mustafa:sitil.2134@localhost/"));
    AMQP::TcpChannel channel(&connection);

    std::cout << "Listening for incoming messages on queue: " << QUEUE_NAME << std::endl;

    // Queue tüketimi ve gelen mesajın işlenmesi
    channel.declareQueue(QUEUE_NAME).onSuccess([&](const std::string &name, uint32_t messagecount, uint32_t consumercount) {
        std::cout << "Declared queue: " << name << std::endl;
    });

    channel.consume(QUEUE_NAME).onReceived([&](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {
        std::string messageBody(message.body(), message.bodySize());
        std::cout << "Received message: " << messageBody << std::endl;

        // Gelen mesajı JSON formatında işle
        auto jsonMessage = nlohmann::json::parse(messageBody);
        std::cout << "Processed contact data: " << jsonMessage.dump(4) << std::endl;

        channel.ack(deliveryTag);
    });

    ev_run(loop, 0);
}
