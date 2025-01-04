#include "JPG.hpp"

namespace image {

JPG JPG::load(const std::filesystem::path& path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if(ifs.fail()) {
        throw std::runtime_error(std::format("[image::JPG::load] ERROR: failed to open file: {}", path.string()));
    }

    std::array<uint8_t, 2> marker{};
    jpg::read_be(ifs, marker[0]);
    jpg::read_be(ifs, marker[1]);
    if(marker[0] == 0xff && marker[1] == 0xd8) {
        std::cerr << std::format("marker {:02x}{:02x}", marker[0], marker[1]) << std::endl;

        while(true) {
            jpg::read_be(ifs, marker[0]);
            jpg::read_be(ifs, marker[1]);
            std::cerr << std::format("marker {:02x}{:02x}", marker[0], marker[1]) << std::endl;
            // EOI
            if(marker[0] == 0xff && marker[1] == 0xd9) {
                break;
            }
            // other markers
            else {
                uint16_t length{};
                jpg::read_be(ifs, length);
                std::cerr << length << std::endl;
                ifs.seekg(length - 2, std::ios::cur);
            }
        }
    }
    else {
        throw std::runtime_error(std::format("[image::JPG::load] ERROR: invalid format image: {}", path.string()));
    }

    return {};
}

}