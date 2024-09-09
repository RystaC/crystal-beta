#pragma once

#include "../common/common.hpp"

namespace vkw {

class Device;

namespace queue {

class CreateInfos {
    struct Info {
        uint32_t family_index;
        std::vector<float> priorities;
    };

    std::vector<Info> infos_;

public:
    friend vkw::Device;

    auto& add(uint32_t family_index, std::vector<float>&& priorities) {
        infos_.emplace_back(
            Info {
                .family_index = family_index,
                .priorities = priorities,
            }
        );

        return *this;
    }
};

}

}