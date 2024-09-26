# PhoneDirectoryBackcpp

**├── application  
│ └── service.hpp  
├── cmake-build-debug  
├── domain  
│   └── contact  
│       ├── contact.hpp  
│       ├── contact-factory.hpp  
│       ├── contact-repository.hpp  
│       └── contact-service.hpp
├── main.cpp  
├── CMakeLists.txt
├── vcpkg.json**

### main.cpp
```c++
#include <crow.h>
#include <pqxx/pqxx>

#include "service.hpp"

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/add_contact")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req){
        // Burada req.body içeriğini kullanarak veritabanına ekleme yapabilirsiniz.
        ApplicationService::addContact(req.body);
        return crow::response(200, "Contact added");
    });

    CROW_ROUTE(app, "/delete_contact")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req){
        // Burada req.body içeriğini kullanarak veritabanından silme yapabilirsiniz.
            ApplicationService::deleteContact(req.body);
        return crow::response(200, "Contact deleted");
    });

    CROW_ROUTE(app, "/get_contacts")
    .methods(crow::HTTPMethod::GET)
    ([&](){
        // Veritabanından tüm kontakları alıp döndürebilirsiniz.
        ApplicationService::getContactList();
        return crow::response(200, "Contact list");
    });

    app.port(8080).multithreaded().run();
}
```

### application/service.hpp
```c++
#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include "nlohmann/json.hpp"

#include "contact-service.hpp"

using json = nlohmann::json;

namespace ApplicationService
{
    pqxx::connection* conn = nullptr;

    inline void connectToDatabase() {
        try {
            conn = new pqxx::connection("dbname=rehber user=mustafa password=sitil.2134 hostaddr=127.0.0.1 port=5432");
        } catch (const std::exception& e) {
            std::cerr << "Connection failed: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }

   inline void addContact(const std::string& req) {
         ApplicationService::connectToDatabase();

    json j = json::parse(req);
    const std::string firstName = j["ad"];
    const std::string lastName = j["soyad"];
    const std::string phone = j["telefon"];

    pqxx::work txn(*conn);
    ContactService::addContact(txn, firstName, lastName, phone);
    txn.commit();
}

inline void deleteContact(const std::string& req) {
      ApplicationService::connectToDatabase();

    json j = json::parse(req);
    const std::string firstName = j["ad"];
    const std::string lastName = j["soyad"];
    const std::string phone = j["telefon"];

    pqxx::work txn(*conn);
    ContactService::deleteContact(txn, firstName, lastName, phone);
    txn.commit();
}

inline std::string getContactList() {
      ApplicationService::connectToDatabase();

    pqxx::work txn(*conn);
    std::string contactList = ContactService::getContactList(txn);
    txn.commit();
    return contactList;
}


inline void cleanup() {
          ApplicationService::connectToDatabase();

        delete conn;
    }
}

#endif //SERVICE_HPP
```

### domain/contact/contact-factory.hpp
```c++
#ifndef CONTACT_FACTORY_HPP
#define CONTACT_FACTORY_HPP

#include <pqxx/pqxx>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace ContactFactory
{
    inline json generateGetContactList(const pqxx::result& res) {
        json jArray = json::array();
        for (auto row : res) {
            json jObject;
            jObject["ad"] = row[0].c_str();
            jObject["soyad"] = row[1].c_str();
            jObject["telefon"] = row[2].c_str();
            jArray.push_back(jObject);
        }
        return jArray.dump();
    }
}

#endif //CONTACT_FACTORY_HPP
```

### domain/contact/contact-repository.hpp
```c++
#ifndef CONTACT_REPOSITORY_HPP
#define CONTACT_REPOSITORY_HPP

#include <pqxx/pqxx>
#include "contact-factory.hpp"

namespace ContactRepository
{
    inline std::string getContactList(pqxx::work& txn) {
        const pqxx::result res = txn.exec("SELECT first_name, last_name, phone FROM contacts");
        return ContactFactory::generateGetContactList(res).dump();
    }

    inline void addContact(pqxx::work& txn, const std::string& firstName, const std::string& lastName, const std::string& phone) {
        txn.exec_params("INSERT INTO contacts (first_name, last_name, phone) VALUES ($1, $2, $3)", firstName, lastName, phone);
    }

    inline void deleteContact(pqxx::work& txn, const std::string& firstName, const std::string& lastName, const std::string& phone) {
        txn.exec_params("DELETE FROM contacts WHERE first_name = $1 AND last_name = $2 AND phone = $3", firstName, lastName, phone);
    }
}

#endif //CONTACT_REPOSITORY_HPP
```

### domain/contact/contact-service.hpp
```c++
#ifndef CONTACT_REPOSITORY_HPP
#define CONTACT_REPOSITORY_HPP

#include <pqxx/pqxx>
#include "contact-factory.hpp"

namespace ContactRepository
{
    inline std::string getContactList(pqxx::work& txn) {
        const pqxx::result res = txn.exec("SELECT first_name, last_name, phone FROM contacts");
        return ContactFactory::generateGetContactList(res).dump();
    }

    inline void addContact(pqxx::work& txn, const std::string& firstName, const std::string& lastName, const std::string& phone) {
        txn.exec_params("INSERT INTO contacts (first_name, last_name, phone) VALUES ($1, $2, $3)", firstName, lastName, phone);
    }

    inline void deleteContact(pqxx::work& txn, const std::string& firstName, const std::string& lastName, const std::string& phone) {
        txn.exec_params("DELETE FROM contacts WHERE first_name = $1 AND last_name = $2 AND phone = $3", firstName, lastName, phone);
    }
}

#endif //CONTACT_REPOSITORY_HPP
```

### CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.29)
project(PhoneDirectoryBackcpp)

set(CMAKE_CXX_STANDARD 20)
find_package(Crow CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(libpqxx REQUIRED)

set(CMAKE_TOOLCHAIN_FILE "/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

include_directories(${CMAKE_SOURCE_DIR}/application
        ${CMAKE_SOURCE_DIR}/domain/contact)

# Uygulama dosyaları
add_executable(${PROJECT_NAME} main.cpp
        application/service.hpp
        domain/contact/contact.hpp
        domain/contact/contact-factory.hpp
        domain/contact/contact-repository.hpp
        domain/contact/contact-service.hpp)

target_link_libraries(${PROJECT_NAME}
        PRIVATE Crow::Crow
        nlohmann_json::nlohmann_json
        libpqxx::pqxx
)
```


