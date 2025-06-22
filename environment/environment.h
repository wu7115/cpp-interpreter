#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "../object/object.h"

namespace object {

class Environment {
private:
    std::unordered_map<std::string, std::shared_ptr<Object>> store;
    std::shared_ptr<Environment> outer;
    
public:
    Environment() {};
    explicit Environment(std::shared_ptr<Environment> outerEnv) : outer(outerEnv) {}

    std::shared_ptr<Object> get(std::string name) {
        if (store.find(name) != store.end()) {
            return store[name];
        } else if (outer != nullptr) {
            return outer->get(name);
        } else {
            return nullptr;
        }
    }

    std::shared_ptr<Object> set(std::string &name, std::shared_ptr<Object> val) {
        store[name] = val;
        return val;
    }
};

inline std::shared_ptr<Environment> newEnvironment() {
    return std::make_shared<Environment>();
}

inline std::shared_ptr<Environment> newEnclosedEnvironment(std::shared_ptr<Environment> outer) {
    return std::make_shared<Environment>(outer);
}

}