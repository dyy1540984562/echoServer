#include "http_handler.h"
#include <cpprest/uri.h>

void HttpHandler::setup_routes(web::http::experimental::listener::http_listener& listener) {
    listener.support(web::http::methods::GET, std::bind(&HttpHandler::handle_get, this, std::placeholders::_1));
}

void HttpHandler::handle_get(web::http::http_request request) {
    auto query = web::uri::split_query(request.relative_uri().query());
    auto name_param = query.find("name");
    std::string name = (name_param != query.end()) ? name_param->second : "World";
    
    std::string result = hello_service_->say_hello(name);
    request.reply(web::http::status_codes::OK, result);
}