#include "PPM.hpp"

namespace image {

PPM PPM::load(const std::filesystem::path& path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if(ifs.fail()) {
        throw std::runtime_error(std::format("[image::PPM::load] ERROR: failed to open file: {}", path.string()));
    } 

    std::string type(2, '\0');
    uint32_t width{};
    uint32_t height{};
    uint32_t max_value{};

    ifs >> type >> width >> height >> max_value;
    std::cerr << std::format("type = {}, width = {}, height = {}, max value = {}", type, width, height, max_value) << std::endl;

    while(true) {
        uint8_t dummy{};
        ifs.read(reinterpret_cast<char*>(&dummy), sizeof(uint8_t));
        if(dummy == '\n') {
            break;
        }
    }
    
    std::vector<uint8_t> image_data(width * height * 3);
    for(size_t i = 0; i < image_data.size(); ++i) {
        ifs.read(reinterpret_cast<char*>(&image_data[i]), sizeof(uint8_t));
    }

    return PPM(std::move(image_data), width, height, 3);
}

}