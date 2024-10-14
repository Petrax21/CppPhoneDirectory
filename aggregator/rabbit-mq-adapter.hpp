#ifndef RABBIT_MQ_ADAPTER_HPP
#define RABBIT_MQ_ADAPTER_HPP

#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <string>
#include <iostream>

class RabbitMQAdapter {
public:
    static void sendMessage(const std::string& message) {
        struct ev_loop *loop = EV_DEFAULT;
        AMQP::LibEvHandler handler(loop);
        AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://mustafa:sitil.2134@localhost/"));
        AMQP::TcpChannel channel(&connection);

        const std::string QUEUE_NAME = "contacts_queue";

        channel.declareQueue(QUEUE_NAME).onSuccess([&](const std::string &name, uint32_t messagecount, uint32_t consumercount) {
            std::cout << "Declared queue: " << name << std::endl;
        });

        channel.publish("", QUEUE_NAME, message);
        ev_run(loop, 0);
    }
};

#endif // RABBIT_MQ_ADAPTER_HPP
