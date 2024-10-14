#ifndef RABBIT_MQ_CONNECTION_HANDLER_HPP
#define RABBIT_MQ_CONNECTION_HANDLER_HPP

#include <amqpcpp.h>
#include <amqpcpp/libboostasio.h>
#include <boost/asio.hpp>

class RabbitMQConnectionHandler : public AMQP::LibBoostAsioHandler
{
public:
    explicit RabbitMQConnectionHandler(boost::asio::io_context& context) : AMQP::LibBoostAsioHandler(context)
    {
    }

    void onReady(AMQP::TcpConnection* connection) override
    {
        std::cout << "RabbitMQ connection has been established" << std::endl;
    }

    void onClosed(AMQP::TcpConnection* connection) override
    {
        std::cerr << "RabbitMQ connection has been interrupted" << std::endl;

        exit(1);
    }

    void onError(AMQP::TcpConnection* connection, const char* message) override
    {
        std::cerr << "RabbitMQ connection error: " << message << std::endl;

        exit(1);
    }
};

#endif // RABBIT_MQ_CONNECTION_HANDLER_HPP

