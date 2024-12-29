#include "BMP.hpp"

namespace image {

BMP BMP::load(const std::filesystem::path& path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if(ifs.fail()) {
        throw std::runtime_error(std::format("[image::BMP::load] ERROR: failed to open file: {}", path.string()));
    }

    auto file_header = bmp::read_file_header(ifs);
    std::cerr << std::format("type = {}, size = {}, offset = {}", file_header.type, file_header.size, file_header.offset) << std::endl;

    auto info_header = bmp::read_info_header(ifs);
    std::cerr << std::format("size = {}, width = {}, height = {}, bit count = {}, comp = {}, palette = {}", info_header.size, info_header.width, info_header.height, info_header.bit_count, info_header.compression, info_header.palette_count) << std::endl;

    ifs.seekg(file_header.offset, std::ios::beg);
    uint32_t pixel{};

    std::vector<uint8_t> image_data(info_header.width * std::abs(info_header.height) * 4);
    for(size_t i = 0; i < image_data.size(); i += 4) {
        uint32_t pixel{};
        ifs.read(reinterpret_cast<char*>(&pixel), sizeof(uint32_t));
        image_data[i + 0] = static_cast<uint8_t>((pixel & 0x00ff0000) >> 16);
        image_data[i + 1] = static_cast<uint8_t>((pixel & 0x0000ff00) >> 8);
        image_data[i + 2] = static_cast<uint8_t>(pixel & 0x000000ff);
        image_data[i + 3] = static_cast<uint8_t>((pixel & 0xff000000) >> 24);
    }

    return BMP(std::move(image_data), info_header.width, std::abs(info_header.height), 4);
}

}