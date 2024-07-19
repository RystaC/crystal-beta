#pragma once

#include "common.hpp"

namespace vkw {

template<typename T>
class Buffer {
    std::shared_ptr<DeviceEntity> device_;
    VkBuffer buffer_;
    VkDeviceMemory memory_;

    using value_type_ = T;

public:
    Buffer(std::shared_ptr<DeviceEntity> device, VkBuffer&& buffer, VkDeviceMemory&& memory) noexcept : device_(device), buffer_(buffer), memory_(memory) {}
    ~Buffer() noexcept {
        vkFreeMemory(*device_, memory_, nullptr);
        vkDestroyBuffer(*device_, buffer_, nullptr);
    }

    operator VkBuffer() const noexcept { return buffer_; }
};

// template<typename T>
// class VertexBuffer : public Buffer<T> {
//     std::vector<VkVertexInputAttributeDescription> attribute_descs_;

// public:
//     VertexBuffer(std::shared_ptr<DeviceEntity> device, VkBuffer&& buffer, VkDeviceMemory&& memory, std::vector<VkVertexInputAttributeDescription>&& descs) :
//         Buffer(device, buffer, memory), attribute_descs_(descs) {}
// };

// namespace private_ {

// template<typename U>
// struct IndexType_ {
//     static_assert([]{ return false; }, "index buffer type expected to uint16_t or uint32_t. uint8 index feature is not avaliable.");
// };
// template<>
// struct IndexType_<uint16_t> {
//     static constexpr VkIndexType index_type = VK_INDEX_TYPE_UINT16;
// };
// template<>
// struct IndexType_<uint32_t> {
//     static constexpr VkIndexType index_type = VK_INDEX_TYPE_UINT32;
// };

// }

// template<typename T>
// class IndexBuffer : public Buffer<T> {
//     static constexpr VkIndexType static_index_type_ = private_::IndexType_<T>::index_type;

//     VkIndexType index_type_;

// public:
//     IndexBuffer(std::shared_ptr<DeviceEntity> device, VkBuffer&& buffer, VkDeviceMemory&& memory) : Buffer(device, buffer, memory), index_type_(static_index_type_) {}

//     auto& index_type() const noexcept { return index_type_; }
// };

}