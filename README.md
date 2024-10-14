# PhoneDirectoryBackcpp

**├── api-gateway  
│ └── gateway.cpp  
│ └── CMakeLists.txt  
│ └── vcpkg.json    
├── aggregator
│ └── aggregator.cpp  
│ └── CMakeLists.txt  
│ └── vcpkg.json  
├── application   
│ └── service.hpp  
├── cmake-build-debug  
├── domain  
│   └── contact
├── microservice
│   └── contact
│       ├── contact.hpp  
│       ├── contact-factory.hpp  
│       ├── contact-repository.hpp  
│       └── contact-service.hpp
├── rabbitmq  
│   └── rabbit-mq-adapter.hpp  
│   └── rabbit-mq-connection-handler.hpp  
├── main.cpp  
├── CMakeLists.txt
├── vcpkg.json**

### main.cpp
```c++
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

    amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest");
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
```

### aggregator/aggregator.cpp
```c++
#include <crow.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <nlohmann/json.hpp>
#include "../rabbitmq/rabbit-mq-connection-handler.hpp"
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

    ev_run(loop, 0);
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/aggregator/add_contact")
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

    CROW_ROUTE(app, "/aggregator/delete_contact")
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

    CROW_ROUTE(app, "/aggregator/get_contacts")
    .methods(crow::HTTPMethod::GET)
    ([]() {

        return crow::response(200, "Getting contact list");
    });

    app.port(8082).multithreaded().run();
}
```


### api-gateway/gateway.cpp
```c++
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
```

### api-gateway/CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.29)
project(ApiGateway)

set(CMAKE_CXX_STANDARD 20)
find_package(Crow CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(libpqxx REQUIRED)
find_package(amqpcpp REQUIRED)
find_package(rabbitmq-c CONFIG REQUIRED)

set(CMAKE_TOOLCHAIN_FILE "/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
set(RabbitMQ_DIR
        rabbitmq/rabbit-mq-adapter.hpp
        rabbitmq/rabbit-mq-connection-handler.hpp
)

add_executable(${PROJECT_NAME} aggregator.cpp
)


target_link_libraries(${PROJECT_NAME}
        PRIVATE Crow::Crow
        nlohmann_json::nlohmann_json
        libpqxx::pqxx
        amqpcpp
        rabbitmq::rabbitmq-static
)
```

### application/service.hpp
```c++
#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <pqxx/pqxx>
#include <string>
#include <vector>
#include "contact.hpp"
#include "contact-repository.hpp"

namespace ApplicationService
{
    inline void connectToDatabase(pqxx::connection*& conn) {
        try {
            conn = new pqxx::connection("dbname=rehber user=mustafa password=sitil.2134 hostaddr=127.0.0.1 port=5432");
        } catch (const std::exception& e) {
            std::cerr << "Connection failed: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    inline void addContact(const std::string& firstName, const std::string& lastName, const std::string& phone) {
        pqxx::connection* conn = nullptr;
        connectToDatabase(conn);
        pqxx::work txn(*conn);
        ContactRepository::addContact(txn, firstName, lastName, phone);
        txn.commit();
        delete conn;
    } // smart pointer araştırma

    inline void deleteContact(const std::string& firstName, const std::string& lastName, const std::string& phone) {
        pqxx::connection* conn = nullptr;
        connectToDatabase(conn);
        pqxx::work txn(*conn);
        ContactRepository::deleteContact(txn, firstName, lastName, phone);
        txn.commit();
        delete conn;
    }

    inline std::vector<Contact> getContactList() {
        pqxx::connection* conn = nullptr;
        connectToDatabase(conn);
        pqxx::work txn(*conn);
        auto contacts = ContactRepository::getContactList(txn);
        txn.commit();
        delete conn;
        return contacts;
    }
}

#endif // SERVICE_HPP
```

### microservice/contact/contact.hpp
```c++
#ifndef CONTACT_HPP
#define CONTACT_HPP

#include <string>
#include <utility>

class Contact {
public:
    Contact(std::string  firstName, std::string  lastName, std::string  phone)
        : firstName(std::move(firstName)), lastName(std::move(lastName)), phone(std::move(phone)) {}

    [[nodiscard]] std::string getFirstName() const { return firstName; }
    [[nodiscard]] std::string getLastName() const { return lastName; }
    [[nodiscard]] std::string getPhone() const { return phone; }

private: // public
    std::string firstName;
    std::string lastName;
    std::string phone;
};

#endif // CONTACT_HPP
```


### microservice/contact/contact-factory.hpp
```c++
#ifndef CONTACT_FACTORY_HPP
#define CONTACT_FACTORY_HPP

#include <pqxx/pqxx>
#include <vector>
#include "contact.hpp"

namespace ContactFactory // insert contact burada 0
{
    inline std::vector<Contact> generateGetContactList(const pqxx::result& res) {
        std::vector<Contact> contacts;
        for (auto row : res) {
            Contact contact(row[0].c_str(), row[1].c_str(), row[2].c_str());
            contacts.push_back(contact);
        }
        return contacts;
    }
}

#endif // CONTACT_FACTORY_HPP
```

### microservice/contact/contact-repository.hpp
```c++
#ifndef CONTACT_REPOSITORY_HPP
#define CONTACT_REPOSITORY_HPP

#include <pqxx/pqxx>
#include <vector>
#include "contact.hpp"
#include "contact-factory.hpp"

namespace ContactRepository // GET UPDATE
{
    inline std::vector<Contact> getContactList(pqxx::work& txn) {
        const pqxx::result res = txn.exec("SELECT first_name, last_name, phone FROM contacts");
        return ContactFactory::generateGetContactList(res);
    }

    inline void addContact(pqxx::work& txn, const std::string& firstName, const std::string& lastName, const std::string& phone) {
        txn.exec_params("INSERT INTO contacts (first_name, last_name, phone) VALUES ($1, $2, $3)", firstName, lastName, phone);
    }
 // id yeterli
    inline void deleteContact(pqxx::work& txn, const std::string& firstName, const std::string& lastName, const std::string& phone) {
        txn.exec_params("DELETE FROM contacts WHERE first_name = $1 AND last_name = $2 AND phone = $3", firstName, lastName, phone);
    }
}

#endif // CONTACT_REPOSITORY_HPP
```

### microservice/contact/contact-service.hpp
```c++
#ifndef CONTACT_SERVICE_HPP
#define CONTACT_SERVICE_HPP

#include "contact-repository.hpp"

namespace ContactService // factory gidicek factoryde contact oluşturulacak //
{
    inline std::string getContactList(pqxx::work& txn) {
        return ContactRepository::getContactList(txn);
    }

    inline void addContact(pqxx::work& txn, const std::string& firstName, const std::string& lastName, const std::string& phone) {
        ContactRepository::addContact(txn, firstName, lastName, phone);
    }

    inline void deleteContact(pqxx::work& txn, const std::string& firstName, const std::string& lastName, const std::string& phone) {
        ContactRepository::deleteContact(txn, firstName, lastName, phone);
    }
};

#endif //CONTACT_SERVICE_HPP
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.29)
project(PhoneDirectoryBackcpp)

set(CMAKE_CXX_STANDARD 20)
find_package(Crow CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(libpqxx REQUIRED)
find_package(amqpcpp REQUIRED)
find_package(rabbitmq-c CONFIG REQUIRED)

set(CMAKE_TOOLCHAIN_FILE "/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
set(RabbitMQ_DIR
        rabbitmq/rabbit-mq-adapter.hpp
        rabbitmq/rabbit-mq-connection-handler.hpp
)


include_directories(${CMAKE_SOURCE_DIR}/application
        ${CMAKE_SOURCE_DIR}/domain/contact
        ${CMAKE_SOURCE_DIR}/api-gateway
        ${CMAKE_SOURCE_DIR}/rabbitmq
)

add_subdirectory(
        api-gateway
)

# Uygulama dosyaları
add_executable(${PROJECT_NAME} main.cpp
        microservice/contact/application/service.hpp
        microservice/contact/domain/contact.hpp
        microservice/contact/domain/contact-factory.hpp
        microservice/contact/domain/contact-repository.hpp
        microservice/contact/domain/contact-service.hpp
        api-gateway/gateway.cpp
        rabbitmq/rabbit-mq-adapter.hpp
        rabbitmq/rabbit-mq-connection-handler.hpp
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE Crow::Crow
        nlohmann_json::nlohmann_json
        libpqxx::pqxx
        amqpcpp
        rabbitmq::rabbitmq-static
)
```