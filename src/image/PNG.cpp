#include "PNG.hpp"

namespace image {

PNG PNG::load(const std::filesystem::path& path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if(ifs.fail()) {
        throw std::runtime_error(std::format("[image::PNG::load] ERROR: failed to open file: {}", path.string()));
    } 

    constexpr uint8_t expected_signature[8] = {
        0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a,
    };

    uint8_t signature[8]{};
    ifs.read(reinterpret_cast<char*>(signature), sizeof(uint8_t) * 8);

    for(auto i = 0; i < 8; ++i) {
        if(signature[i] != expected_signature[i]) {
            throw std::runtime_error(std::format("[image::PNG::load] ERROR: unexpected file format. expected PNG file."));
        }
    }

    png::IHDR ihdr{};
    std::vector<png::IDAT> data_chunks{};
    size_t total_data_length{};

    while(true) {
        uint32_t length{};
        std::string chunk_type(4, '\0');
        uint32_t crc32{};
        read_be_(ifs, length);
        ifs.read(chunk_type.data(), sizeof(uint8_t) * 4);

        if(chunk_type == "IHDR") {
            ihdr = std::move(png::read_ihdr(ifs));
            read_be_(ifs, crc32);
        }
        else if(chunk_type == "IDAT") {
            total_data_length += length;
            data_chunks.emplace_back(png::read_idat(ifs, length));
            read_be_(ifs, crc32);
        }
        else if(chunk_type == "IEND") {
            read_be_(ifs, crc32);
            break;
        }
        else {
            ifs.seekg(length, std::ios::cur);
            read_be_(ifs, crc32);
        }
    }

    auto deflate = png::concat_idat(std::move(data_chunks), total_data_length);
    size_t start_index{};
    auto cinfo = deflate.data[1];
    if(cinfo & 0x20) {
        start_index = 48;
    }
    else {
        start_index = 16;
    }

    enum ReadState {
        HEADER,
        NO_COMPRESSED,
        FIXED_HUFFMAN,
        CUSTOM_HUFFMAN,
    };

    std::vector<uint8_t> image_data{};
    image_data.reserve(ihdr.width * ihdr.height * 4);
    std::array<uint8_t, 258> tmp_data{};

    ReadState state = HEADER;
    bool is_final_block = false;
    for(size_t i = start_index; i < deflate.size();) {
        if(state == HEADER) {
            is_final_block = bool(deflate.read_bits(1, i));
            auto btype = deflate.read_bits(2, i);
            if(btype == 0) {
                state = NO_COMPRESSED;
            }
            else if(btype == 1) {
                state = FIXED_HUFFMAN;
            }
            else if(btype == 2) {
                state = CUSTOM_HUFFMAN;
            }
        }
        else if(state == CUSTOM_HUFFMAN) {
            png::CustomHuffman huffman_table{};

            huffman_table.read_header(deflate, i);
            huffman_table.read_code_length_codes(deflate, i);
            huffman_table.read_character_length_codes(deflate, i);
            huffman_table.read_distance_length_codes(deflate, i);

            while(true) {
                auto character_code = huffman_table.characters.read_code(deflate, i);
                if(character_code == 256) {
                    break;
                }
                else if(character_code >= 257) {
                    auto match = png::LZSS::MATCH_TABLE[character_code - 257];
                    auto match_extend = deflate.read_bits(match.second, i);
                    auto distance_code = huffman_table.distances.read_code(deflate, i);
                    auto distance = png::LZSS::DISTANCE_TABLE[distance_code];
                    auto distance_extend = deflate.read_bits(distance.second, i);
                    auto match_length = match.first + match_extend;
                    auto match_distance = distance.first + distance_extend;
                    auto copy_length = std::min(match_length, match_distance);
                    for(uint32_t j = 0; j < copy_length; ++j) {
                        tmp_data[j] = image_data[image_data.size() - match_distance];
                    }
                    for(uint32_t j = 0; j < match_length; ++j) {
                        image_data.push_back(tmp_data[j % copy_length]);
                    }
                }
                else {
                    image_data.push_back(static_cast<uint8_t>(character_code));
                }
            }
            if(is_final_block) {
                break;
            }
            else {
                state = HEADER;
            }
        }
        else {
            ++i;
        }
    }

    std::cerr << std::format("data size = {}", image_data.size()) << std::endl;

    return PNG{};
}

}