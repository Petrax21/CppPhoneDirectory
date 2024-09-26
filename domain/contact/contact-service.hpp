#ifndef CONTACT_SERVICE_HPP
#define CONTACT_SERVICE_HPP

#include "contact-repository.hpp"

namespace ContactService
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
