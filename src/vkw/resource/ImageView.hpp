#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace resource {

class ImageView final {
    std::shared_ptr<resource::Device> device_;
    VkImageView view_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkImageView;

    ImageView() noexcept {}
    ImageView(std::shared_ptr<resource::Device> device, VkImageView&& view, const VkAllocationCallbacks* allocator) noexcept : device_(device), view_(view), allocator_(allocator) {};
    ~ImageView() noexcept {
        if(device_) vkDestroyImageView(*device_, view_, allocator_);
    }
    ImageView(const ImageView& rhs) = delete;
    auto& operator=(const ImageView& rhs) = delete;
    ImageView(ImageView&& rhs) = default;
    ImageView& operator=(ImageView&& rhs) = default;

    operator VkImageView() const noexcept { return view_; }
};

}

}