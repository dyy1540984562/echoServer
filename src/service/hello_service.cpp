#include "hello_service.h"
#include "../common.h"

std::string HelloService::say_hello(const std::string& name) {
    LOG_INFO("Saying hello to " << name);
    return hello_dao_->get_greeting(name);
}