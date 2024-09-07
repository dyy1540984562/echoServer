#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

int main() {
    http_client client(U("http://localhost:8080"));

    uri_builder builder(U("/hello"));
    builder.append_query(U("name"), U("World"));

    auto task = client.request(methods::GET, builder.to_string())
        .then([](http_response response) {
            if (response.status_code() == status_codes::OK) {
                return response.extract_string();
            }
            throw std::runtime_error("HTTP request failed");
        })
        .then([](std::string body) {
            std::cout << "Received: " << body << std::endl;
        });

    try {
        task.wait();
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}