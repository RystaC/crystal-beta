#include <memory>
#include <vector>

#include "common.hpp"

namespace vkw {

class InstanceEntity {
    VkInstance instance_;

public:
    InstanceEntity(VkInstance&& instance) noexcept : instance_(instance) {}
    ~InstanceEntity() noexcept {
        vkDestroyInstance(instance_, nullptr);
    }
};

class Instance {
    std::shared_ptr<InstanceEntity> instance_;

public:
    Instance() noexcept : instance_() {}
    ~Instance() noexcept {}

    VkResult init(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);
};

}