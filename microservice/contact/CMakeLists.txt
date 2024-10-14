cmake_minimum_required(VERSION 3.29)
project(contact)

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


include_directories(${CMAKE_SOURCE_DIR}/domain
        ${CMAKE_SOURCE_DIR}/application
)

add_executable(${PROJECT_NAME}
        domain/contact.hpp
        domain/contact-factory.hpp
        domain/contact-repository.hpp
        domain/contact-service.hpp
        application/service.hpp
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE Crow::Crow
        nlohmann_json::nlohmann_json
        libpqxx::pqxx
        amqpcpp
        rabbitmq::rabbitmq-static
)