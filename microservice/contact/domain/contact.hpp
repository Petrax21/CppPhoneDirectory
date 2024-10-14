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