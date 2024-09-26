#ifndef CONTACT_REPOSITORY_HPP
#define CONTACT_REPOSITORY_HPP

#include <pqxx/pqxx>
#include "contact-factory.hpp"

namespace ContactRepository
{
    // Kişi listesini alma
    inline std::string getContactList(pqxx::work& txn) {
        const pqxx::result res = txn.exec("SELECT first_name, last_name, phone FROM contacts");
        return ContactFactory::generateGetContactList(res);
    }

    // Kişi ekleme
    inline void addContact(pqxx::work& txn, const std::string& firstName, const std::string& lastName, const std::string& phone) {
        txn.exec_params("INSERT INTO contacts (first_name, last_name, phone) VALUES ($1, $2, $3)", firstName, lastName, phone);
    }

    // Kişi silme
    inline void deleteContact(pqxx::work& txn, const std::string& firstName, const std::string& lastName, const std::string& phone) {
        txn.exec_params("DELETE FROM contacts WHERE first_name = $1 AND last_name = $2 AND phone = $3", firstName, lastName, phone);
    }
}

#endif //CONTACT_REPOSITORY_HPP
