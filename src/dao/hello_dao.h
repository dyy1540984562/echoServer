#pragma once
#include <string>

class HelloDao {
public:
    std::string get_greeting(const std::string& name);
};