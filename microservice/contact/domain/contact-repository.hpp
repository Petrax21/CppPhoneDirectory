#ifndef CONTACT_REPOSITORY_HPP
#define CONTACT_REPOSITORY_HPP

#include <pqxx/pqxx>
#include <vector>
#include "contact.hpp"
#include "contact-factory.hpp"

namespace ContactRepository
{
    inline std::vector<Contact> getContactList(pqxx::work& txn) {
        const pqxx::result res = txn.exec("SELECT id, first_name, last_name, phone FROM contacts");
        return ContactFactory::generateGetContactList(res);
    }

    inline Contact getContact(pqxx::work& txn, int id) {
        const pqxx::result res = txn.exec_params("SELECT id, first_name, last_name, phone FROM contacts WHERE id = $1", id);
        return ContactFactory::(res[0]);
    }

    inline void addContact(pqxx::work& txn, const std::string& firstName, const std::string& lastName, const std::string& phone) {
        txn.exec_params("INSERT INTO contacts (first_name, last_name, phone) VALUES ($1, $2, $3)", firstName, lastName, phone);
    }

    inline void deleteContact(pqxx::work& txn, int id) {
        txn.exec_params("DELETE FROM contacts WHERE id = $1", id);
    }

    inline void updateContact(pqxx::work& txn, int id, const std::string& firstName, const std::string& lastName, const std::string& phone) {
        txn.exec_params("UPDATE contacts SET first_name = $1, last_name = $2, phone = $3 WHERE id = $4", firstName, lastName, phone, id);
    }
}

#endif // CONTACT_REPOSITORY_HPP
