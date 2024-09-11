#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class ImageView final {
    std::shared_ptr<objects::Device> device_;
    VkImageView view_;

public:
    using object_type = VkImageView;

    ImageView() noexcept {}
    ImageView(std::shared_ptr<objects::Device> device, VkImageView&& view) noexcept : device_(device), view_(view) {};
    ~ImageView() noexcept {
        if(device_) vkDestroyImageView(*device_, view_, nullptr);
    }
    ImageView(const ImageView& rhs) = delete;
    auto& operator=(const ImageView& rhs) = delete;
    ImageView(ImageView&& rhs) = default;
    ImageView& operator=(ImageView&& rhs) = default;

    operator VkImageView() const noexcept { return view_; }
};

}

}