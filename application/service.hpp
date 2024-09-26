#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include "nlohmann/json.hpp"
#include "contact-service.hpp"
#include "contact-repository.hpp"

using json = nlohmann::json;

namespace ApplicationService
{
    pqxx::connection* conn = nullptr;

    inline void connectToDatabase() {
        if (conn == nullptr) {  // Bağlantının zaten açık olup olmadığını kontrol et
            try {
                conn = new pqxx::connection("dbname=rehber user=mustafa password=sitil.2134 hostaddr=127.0.0.1 port=5432");
            } catch (const std::exception& e) {
                std::cerr << "Bağlantı hatası: " << e.what() << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    inline void addContact(const std::string& req) {
        connectToDatabase();

        json j = json::parse(req);
        const std::string firstName = j["ad"];
        const std::string lastName = j["soyad"];
        const std::string phone = j["telefon"];

        pqxx::work txn(*conn);
        ContactService::addContact(txn, firstName, lastName, phone);
        txn.commit();
    }

    inline void deleteContact(const std::string& req) {
        connectToDatabase();

        json j = json::parse(req);
        const std::string firstName = j["ad"];
        const std::string lastName = j["soyad"];
        const std::string phone = j["telefon"];

        pqxx::work txn(*conn);
        ContactService::deleteContact(txn, firstName, lastName, phone);
        txn.commit();
    }

    inline std::string getContactList() {
        connectToDatabase();

        pqxx::work txn(*conn);
        std::string contactList = ContactService::getContactList(txn);
        txn.commit();
        return contactList;
    }

    inline void cleanup() {
        if (conn != nullptr) {
            delete conn;  // Bağlantıyı serbest bırak
            conn = nullptr;  // İşaretçiyi null yap
        }
    }

}

#endif //SERVICE_HPP
