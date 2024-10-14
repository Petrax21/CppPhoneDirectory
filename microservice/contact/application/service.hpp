#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <pqxx/pqxx>
#include <string>
#include <vector>
#include "domain/contact.hpp"
#include "domain/contact-repository.hpp"
#include "rabbit-mq-adapter.hpp"


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
        RabbitMQAdapter::sendMessage(contactInfo.dump());
        delete conn;
    } // smart pointer yapılabilir

    inline void deleteContact(const std::string& firstName, const std::string& lastName, const std::string& phone) {
        pqxx::connection* conn = nullptr;
        connectToDatabase(conn);
        pqxx::work txn(*conn);
        ContactRepository::deleteContact(txn, firstName, lastName, phone);
        txn.commit();
        RabbitMQAdapter::sendMessage(contactInfo.dump());
        delete conn;
    }

    inline std::vector<Contact> getContactList() {
        pqxx::connection* conn = nullptr;
        connectToDatabase(conn);
        pqxx::work txn(*conn);
        auto contacts = ContactRepository::getContactList(txn);
        txn.commit();
        RabbitMQAdapter::sendMessage(contactInfo.dump());
        delete conn;
        return contacts;
    }
}

#endif // SERVICE_HPP