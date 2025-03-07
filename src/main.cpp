#include "graphics/Engine.hpp"
#include "graphics/Logger.hpp"

int main(int argc, const char* argv[]) {
    // TODO: control destruction order of singletons.
    // CustomAllocator::~CustomAllocator depends on Logger.
    graphics::HostMemoryAllocator::get();
    
    std::ofstream log_stream("test.log", std::ios::out);

    auto& logger = graphics::Logger::get(log_stream);
    LOG_DEBUG(std::format("debug message"));
    LOG_INFO(std::format("info message"));
    LOG_WARNING(std::format("warning message"));
    LOG_ERROR(std::format("error message"));
    LOG_FATAL(std::format("fatal message"));

    auto engine = graphics::Engine::initialize();
    graphics::HostMemoryAllocator::get().print_summary();

    using namespace graphics::literals;
    std::cerr << 5_gB  << std::endl;
    return 0;
}