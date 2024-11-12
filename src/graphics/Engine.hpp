#pragma once

#include <memory>

#include "../vkw/vkw.hpp"

namespace graphics {

class Engine {
    std::unique_ptr<vkw::Instance> instance_;
    std::unique_ptr<vkw::Device> device_;

public:
};

}