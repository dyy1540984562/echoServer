#pragma once
#include <string>
#include <memory>
#include "../dao/hello_dao.h"


class HelloService {
public:
    HelloService(std::shared_ptr<HelloDao> dao) : hello_dao_(dao) {}
    std::string say_hello(const std::string& name);

private:
    std::shared_ptr<HelloDao> hello_dao_;
};