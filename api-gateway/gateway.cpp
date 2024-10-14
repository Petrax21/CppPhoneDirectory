#include <crow.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include "rabbit-mq-connection-handler.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

const std::string QUEUE_NAME = "contacts_queue";

void sendMessageToQueue(const std::string &message) {
    struct ev_loop *loop = EV_DEFAULT;
    AMQP::LibEvHandler handler(loop);
    AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://mustafa:sitil.2134@localhost/"));
    AMQP::TcpChannel channel(&connection);

    channel.declareQueue(QUEUE_NAME).onSuccess([&](const std::string &name, uint32_t messagecount, uint32_t consumercount) {
        std::cout << "Declared queue: " << name << std::endl;
    });

    channel.publish("", QUEUE_NAME, message);

    ev_run(loop, 0); /
}

int main() {
    crow::SimpleApp app;


    CROW_ROUTE(app, "/api/add_contact")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req) {
        auto jsonBody = nlohmann::json::parse(req.body);

        std::string firstName = jsonBody["firstName"];
        std::string lastName = jsonBody["lastName"];
        std::string phone = jsonBody["phone"];

        nlohmann::json message;
        message["action"] = "add";
        message["firstName"] = firstName;
        message["lastName"] = lastName;
        message["phone"] = phone;

        sendMessageToQueue(message.dump());

        return crow::response(200, "Message sent to queue for adding contact");
    });


    CROW_ROUTE(app, "/api/delete_contact")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req) {
        auto jsonBody = nlohmann::json::parse(req.body);

        std::string firstName = jsonBody["firstName"];
        std::string lastName = jsonBody["lastName"];
        std::string phone = jsonBody["phone"];

        nlohmann::json message;
        message["action"] = "delete";
        message["firstName"] = firstName;
        message["lastName"] = lastName;
        message["phone"] = phone;

        sendMessageToQueue(message.dump());

        return crow::response(200, "Message sent to queue for deleting contact");
    });


    CROW_ROUTE(app, "/api/get_contacts")
    .methods(crow::HTTPMethod::GET)
    ([]() {

        return crow::response(200, "Getting contact list from queue");
    });

    app.port(8081).multithreaded().run();
}