#pragma once
#include <cpprest/http_listener.h>
#include "../service/hello_service.h"

class HttpHandler {
public:
    HttpHandler(std::shared_ptr<HelloService> service) : hello_service_(service) {}
    void setup_routes(web::http::experimental::listener::http_listener& listener);

private:
    std::shared_ptr<HelloService> hello_service_;
    void handle_get(web::http::http_request request);
};