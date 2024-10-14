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