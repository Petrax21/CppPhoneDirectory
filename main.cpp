#include <crow.h>
#include <pqxx/pqxx>
#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include "api-gateway/gateway.cpp"
#include "rabbit-mq-adapter.hpp"
#include "rabbit-mq-connection-handler.hpp"

#include "service.hpp"

void sendToRabbitMQ(const std::string& message) {
    amqp_connection_state_t conn;
    conn = amqp_new_connection();

    amqp_socket_t *socket = amqp_tcp_socket_new(conn);
    if (!socket) {
        std::cerr << "RabbitMQ socket oluşturulamadı.\n";
        return;
    }

    int status = amqp_socket_open(socket, "localhost", 5672);
    if (status) {
        std::cerr << "RabbitMQ bağlantısı açılamadı.\n";
        return;
    }

    amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "mustafa", "mustafa");
    amqp_channel_open(conn, 1);
    amqp_get_rpc_reply(conn);

    amqp_basic_publish(conn, 1, amqp_cstring_bytes(""), amqp_cstring_bytes("queue_name"),
        0, 0, nullptr, amqp_cstring_bytes(message.c_str()));

    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/add_contact")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req){
        auto j = nlohmann::json::parse(req.body);
        ApplicationService::addContact(j["ad"], j["soyad"], j["telefon"]);
        return crow::response(200, "Contact added");
    });

    CROW_ROUTE(app, "/delete_contact")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req){
        auto j = nlohmann::json::parse(req.body);
        ApplicationService::deleteContact(j["ad"], j["soyad"], j["telefon"]);
        return crow::response(200, "Contact deleted");
    });

    CROW_ROUTE(app, "/get_contacts")
    .methods(crow::HTTPMethod::GET)
    ([&](){
        auto contacts = ApplicationService::getContactList();
        nlohmann::json jArray = nlohmann::json::array();
        for (const auto& contact : contacts) {
            nlohmann::json jObject;
            jObject["ad"] = contact.getFirstName();
            jObject["soyad"] = contact.getLastName();
            jObject["telefon"] = contact.getPhone();
            jArray.push_back(jObject);
        }
        return crow::response(200, jArray.dump());
    });

    app.port(8080).multithreaded().run();
}


// root config