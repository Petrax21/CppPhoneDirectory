#include <crow.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include "service.hpp"

using json = nlohmann::json;

void routeApiGateway(crow::SimpleApp& app) {
    // Kişi ekleme isteğini yönlendirme
    CROW_ROUTE(app, "/api/add_contact")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req){
        // Gelen POST isteği veritabanına yönlendirilecek
        ApplicationService::addContact(req.body);
        return crow::response(200, "Contact added via API Gateway");
    });

    // Kişi silme isteğini yönlendirme
    CROW_ROUTE(app, "/api/delete_contact")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req){
        ApplicationService::deleteContact(req.body);
        return crow::response(200, "Contact deleted via API Gateway");
    });

    // Tüm kişileri alma isteğini yönlendirme
    CROW_ROUTE(app, "/api/get_contacts")
    .methods(crow::HTTPMethod::GET)
    ([&](){
        std::string contactList = ApplicationService::getContactList();
        return crow::response(200, contactList);
    });
}
