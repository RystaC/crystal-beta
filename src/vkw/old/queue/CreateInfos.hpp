#pragma once

#include "../common/common.hpp"

namespace vkw {

class Device;

namespace queue {

class CreateInfos {
    std::unordered_map<uint32_t, std::vector<float>> infos_;

public:
    friend vkw::Device;

    auto& add(uint32_t family_index, std::vector<float>&& priorities) {
        infos_[family_index].insert(infos_[family_index].end(), priorities.begin(), priorities.end());

        return *this;
    }
};

}

}