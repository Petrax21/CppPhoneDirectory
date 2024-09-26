cmake_minimum_required(VERSION 3.29)
project(PhoneDirectoryBackcpp)

set(CMAKE_CXX_STANDARD 20)
find_package(Crow CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(libpqxx REQUIRED)
find_package(amqpcpp REQUIRED)
find_package(rabbitmq-c CONFIG REQUIRED)

set(CMAKE_TOOLCHAIN_FILE "/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
set(RabbitMQ_DIR "/path/to/rabbitmq-c")


include_directories(${CMAKE_SOURCE_DIR}/application
        ${CMAKE_SOURCE_DIR}/domain/contact
        ${CMAKE_SOURCE_DIR}/api-gateway
)

# Uygulama dosyaları
add_executable(${PROJECT_NAME} main.cpp
        application/service.hpp
        domain/contact/contact.hpp
        domain/contact/contact-factory.hpp
        domain/contact/contact-repository.hpp
        domain/contact/contact-service.hpp
        api-gateway/gateway.cpp)

target_link_libraries(${PROJECT_NAME}
        PRIVATE Crow::Crow
        nlohmann_json::nlohmann_json
        libpqxx::pqxx
        amqpcpp
        rabbitmq::rabbitmq-static
)