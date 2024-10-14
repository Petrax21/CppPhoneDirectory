cmake_minimum_required(VERSION 3.29)
project(Aggregator)

set(CMAKE_CXX_STANDARD 20)

# Vcpkg kullanarak gerekli paketleri bulma
find_package(Crow CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(amqpcpp REQUIRED)
find_package(rabbitmq-c CONFIG REQUIRED)

# RabbitMQ başlık dosyaları
set(RabbitMQ_DIR
        rabbitmq/rabbit-mq-adapter.hpp
        rabbitmq/rabbit-mq-connection-handler.hpp
)

# Ana proje için derleme ayarları
add_executable(${PROJECT_NAME} aggregator.cpp
        aggregator.hpp
        rabbit-mq-adapter.hpp
)

# Bağımlılıkları projeye bağlama
target_link_libraries(${PROJECT_NAME}
        PRIVATE Crow::Crow
        nlohmann_json::nlohmann_json
        amqpcpp
        rabbitmq::rabbitmq-static
)
