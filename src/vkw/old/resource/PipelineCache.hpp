#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace resource {

class PipelineCache final {
    std::shared_ptr<resource::Device> device_;
    VkPipelineCache cache_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkPipelineCache;

    PipelineCache() noexcept {}
    PipelineCache(std::shared_ptr<resource::Device> device, VkPipelineCache&& cache, const VkAllocationCallbacks* allocator) noexcept : device_(device), cache_(cache), allocator_(allocator) {}
    ~PipelineCache() noexcept {
        if(device_) vkDestroyPipelineCache(*device_, cache_, allocator_);
    }
    PipelineCache(const PipelineCache& rhs) = delete;
    auto& operator=(const PipelineCache& rhs) = delete;
    PipelineCache(PipelineCache&& rhs) = default;
    PipelineCache& operator=(PipelineCache&& rhs) = default;

    operator VkPipelineCache() const noexcept { return cache_; }

    void merge(const std::vector<VkPipelineCache>& caches) {
        vkMergePipelineCaches(*device_, cache_, size_u32(caches.size()), caches.data());
    }

    void save(const std::filesystem::path& cache_path) {
        size_t data_size{};
        vkGetPipelineCacheData(*device_, cache_, &data_size, nullptr);
        std::vector<char> data(data_size);
        vkGetPipelineCacheData(*device_, cache_, &data_size, reinterpret_cast<void*>(data.data()));

        std::ofstream ofs(cache_path, std::ios::binary);
        if(ofs.fail()) {
            std::cerr << "[vkw::resource::PipelineCache::save] ERROR: failed to open or create file: " << cache_path << std::endl;
            return;
        }

        ofs.write(data.data(), data_size);
        ofs.close();
    }
};

}

}