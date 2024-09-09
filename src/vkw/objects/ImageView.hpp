#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class ImageView {
    std::shared_ptr<objects::Device> device_;
    VkImageView view_;

public:
    using object_type = VkImageView;

    ImageView() noexcept {}
    ImageView(std::shared_ptr<objects::Device> device, VkImageView&& view) noexcept : device_(device), view_(view) {};
    ~ImageView() noexcept {
        vkDestroyImageView(*device_, view_, nullptr);
    }

    operator VkImageView() const noexcept { return view_; }
};

}

}