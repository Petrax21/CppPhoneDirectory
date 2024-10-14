#ifndef RABBIT_MQ_ADAPTER_HPP
#define RABBIT_MQ_ADAPTER_HPP


#include <memory>
#include <functional>
#include <iostream>
#include <amqpcpp.h>
#include <amqpcpp/libboostasio.h>
#include <boost/asio/io_context.hpp>
#include "rabbit-mq-connection-handler.hpp"

class RabbitMQAdapter
{
public:
    using MessageCallback = std::function<void(const std::string_view&, uint64_t, bool)>;

    static RabbitMQAdapter& getInstance()
    {
        static RabbitMQAdapter instance;

        return instance;
    }

    void init(const std::string& messageBroker)
    {
        io_context = std::make_unique<boost::asio::io_context>();
        handler = std::make_unique<RabbitMQConnectionHandler>(*io_context);

        AMQP::Address address(messageBroker);
        connection = std::make_unique<AMQP::TcpConnection>(handler.get(), address);
        channel = std::make_unique<AMQP::TcpChannel>(connection.get());
    }

    void start()
    {
        io_context->run();
    }

    void consume(const std::string& queue, const MessageCallback& callback)
    {
        if (channel)
        {
            channel->declareQueue(queue).onError([queue](const char* message)
            {
                std::cerr << queue << " can not be declared on RabbitMQ, error: " << message << std::endl;
            });

            channel->consume(queue).onReceived([this, callback](const AMQP::Message& message, uint64_t deliveryTag, bool redelivered)
            {
                const std::string body(message.body(), message.bodySize());

                std::thread([body, &callback, deliveryTag, redelivered]()
                {
                    callback(body, deliveryTag, redelivered);
                }).detach();
            });
        }
    }

    void ack(uint64_t deliveryTag)
    {
        if (channel)
        {
            channel->ack(deliveryTag);
        }
    }

    void sendMessage(const std::string& queue, const std::string& message)
    {
        if (channel)
        {
            channel->publish("", queue, message);
        }
    }

private:
    RabbitMQAdapter() = default;
    RabbitMQAdapter(const RabbitMQAdapter&) = delete;
    RabbitMQAdapter& operator=(const RabbitMQAdapter&) = delete;

    std::unique_ptr<boost::asio::io_context> io_context;
    std::unique_ptr<RabbitMQConnectionHandler> handler;
    std::unique_ptr<AMQP::TcpConnection> connection;
    std::unique_ptr<AMQP::TcpChannel> channel;
};

#endif // RABBIT_MQ_ADAPTER_HPP

