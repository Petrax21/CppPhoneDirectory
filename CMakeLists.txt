cmake_minimum_required(VERSION 3.29)
project(PhoneDirectoryBackcpp)

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


include_directories(${CMAKE_SOURCE_DIR}/application
        ${CMAKE_SOURCE_DIR}/microservice/contact
        ${CMAKE_SOURCE_DIR}/api-gateway
        ${CMAKE_SOURCE_DIR}/rabbitmq
)

add_subdirectory(
        api-gateway
)

add_subdirectory(
        aggregator
)

add_subdirectory(
        microservice/contact
)

add_executable(${PROJECT_NAME} main.cpp
        microservice/contact/application/service.hpp
        microservice/contact/domain/contact.hpp
        microservice/contact/domain/contact-factory.hpp
        microservice/contact/domain/contact-repository.hpp
        microservice/contact/domain/contact-service.hpp
        api-gateway/gateway.cpp
        rabbitmq/rabbit-mq-connection-handler.hpp
        rabbitmq/rabbit-mq-adapter.hpp
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE Crow::Crow
        nlohmann_json::nlohmann_json
        libpqxx::pqxx
        amqpcpp
        rabbitmq::rabbitmq-static
)