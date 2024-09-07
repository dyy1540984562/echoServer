#include "hello_service.h"
#include <glog/logging.h>

std::string HelloService::say_hello(const std::string& name) {
    LOG(INFO) << "say hello" << name;
    return hello_dao_->get_greeting(name);
}