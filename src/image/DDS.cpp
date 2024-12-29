#include "DDS.hpp"

namespace image {

DDS DDS::load(const std::filesystem::path& path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if(ifs.fail()) {
        throw std::runtime_error(std::format("[image::DDS::load] ERROR: failed to open file: {}", path.string()));
    } 

    auto header = dds::read_header(ifs);
    auto mipmap_count = (header.flags & 0x00020000) ? std::max(header.mipmap_count, 1u) : 1;

    std::cerr << std::format("width = {}, height = {}, mipmap count = {}, four cc = {:x}", header.width, header.height, mipmap_count, header.four_cc) << std::endl;

    uint32_t block_size = 16;
    uint32_t block_width = header.width;
    uint32_t block_height = header.height;

    if(header.pf_flags & 0x00000004) {
        if(header.four_cc == make_four_cc_("DX10")) {
            std::cerr << "DX10" << std::endl;
            auto header_dx10 = dds::read_header_dx10(ifs);
        }
        else if(header.four_cc == make_four_cc_("DXT1")) {
            std::cerr << "DXT1" << std::endl;
            block_size = 8;
        }
        else if(header.four_cc == make_four_cc_("DXT2")) {
            std::cerr << "DXT2" << std::endl;
        }
        else if(header.four_cc == make_four_cc_("DXT3")) {
            std::cerr << "DXT3" << std::endl;
        } 
        else if(header.four_cc == make_four_cc_("DXT4")) {
            std::cerr << "DXT4" << std::endl;
        }
        else if(header.four_cc == make_four_cc_("DXT5")) {
            std::cerr << "DXT5" << std::endl;
        }
        else if(header.four_cc == make_four_cc_("BC4U")) {
            std::cerr << "BC4U" << std::endl;
        }
        else if(header.four_cc == make_four_cc_("BC4S")) {
            std::cerr << "BC4S" << std::endl;
        }
        else if(header.four_cc == make_four_cc_("BC5U")) {
            std::cerr << "BC5U" << std::endl;
        }
        else if(header.four_cc == make_four_cc_("BC5S")) {
            std::cerr << "BC4S" << std::endl;
        }
        else {
            throw std::runtime_error(std::format("[image::DDS::load] ERROR: unknown FOURCC format: {:x}", header.four_cc));
        }
    }

    uint32_t main_image_size = std::max(1u, (block_width + 3) / 4) * std::max(1u, (block_height + 3) / 4) * block_size;
    std::vector<uint8_t> image_data(main_image_size);

    return DDS(std::move(image_data), block_width, block_height);
}

}