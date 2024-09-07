#include "hello_dao.h"

std::string HelloDao::get_greeting(const std::string& name) {
    return "Hello, " + name + "!";
}