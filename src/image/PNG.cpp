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
        png::read_be(ifs, length);
        ifs.read(chunk_type.data(), sizeof(uint8_t) * 4);

        if(chunk_type == "IHDR") {
            ihdr = std::move(png::read_ihdr(ifs));
            png::read_be(ifs, crc32);
        }
        else if(chunk_type == "IDAT") {
            total_data_length += length;
            data_chunks.emplace_back(png::read_idat(ifs, length));
            png::read_be(ifs, crc32);
        }
        else if(chunk_type == "IEND") {
            png::read_be(ifs, crc32);
            break;
        }
        else {
            ifs.seekg(length, std::ios::cur);
            png::read_be(ifs, crc32);
        }
    }

    std::cerr << std::format("width = {}, height = {}, type = {:03b}", ihdr.width, ihdr.height, ihdr.color_type) << std::endl;

    // concatinate IDAT data chunks as zlib data
    auto deflate = png::concat_idat(std::move(data_chunks), total_data_length);
    size_t start_index{};
    // zlib header
    auto cmf = deflate.data[0];
    auto cinfo = deflate.data[1];
    // have preset dictionary
    if(cinfo & 0x20) {
        start_index = 48;
    }
    else {
        start_index = 16;
    }

    enum ReadState {
        HEADER,
        NOT_COMPRESSED,
        FIXED_HUFFMAN,
        CUSTOM_HUFFMAN,
    };

    // RGB or gray scale
    uint32_t c_count = ihdr.color_type & 2 ? 3 : 1;
    // has alpha channel?
    c_count += ihdr.color_type & 4 ? 1 : 0;

    png::Image image_data(ihdr.width, ihdr.height, c_count);
    // temporary buffer for LZSS
    std::array<uint8_t, 258> tmp_data{};

    ReadState state = HEADER;
    bool is_final_block = false;
    for(size_t i = start_index; i < deflate.size();) {
        if(state == HEADER) {
            is_final_block = bool(deflate.read_bits(1, i));
            auto btype = deflate.read_bits(2, i);
            if(btype == 0) {
                state = NOT_COMPRESSED;
            }
            else if(btype == 1) {
                state = FIXED_HUFFMAN;
            }
            else if(btype == 2) {
                state = CUSTOM_HUFFMAN;
            }
        }
        // BTYPE = 0b00 -> not compressed data
        else if(state == NOT_COMPRESSED) {
            // padding 5 bits
            deflate.read_bits(5, i);

            // data length 16 bits
            uint16_t len = deflate.read_bits(8, i) | (deflate.read_bits(8, i) << 8);
            // length complement
            uint16_t nlen = deflate.read_bits(8, i) | (deflate.read_bits(8, i) << 8);

            // raw data 8 bits each
            for(uint16_t b = 0; b < len; ++b) {
                image_data.add_component(static_cast<uint8_t>(deflate.read_bits(8, i)));
            }

            if(is_final_block) {
                break;
            }
            else {
                state = HEADER;
            }
        }
        // BTYPE = 0b01 -> fixed huffman code
        else if(state == FIXED_HUFFMAN) {
            png::FixedHuffman huffman_table{};

            huffman_table.init();

            while(true) {
                auto character_code = huffman_table.characters.read_code(deflate, i);
                // end of block
                if(character_code == 256) {
                    break;
                }
                // LZSS code
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
                        tmp_data[j] = image_data.get_component(image_data.size() - match_distance + j);
                    }

                    for(uint32_t j = 0; j < match_length; ++j) {
                        image_data.add_component(tmp_data[j % copy_length]);
                    }
                }
                // raw data
                else {
                    image_data.add_component(static_cast<uint8_t>(character_code));
                }
            }

            if(is_final_block) {
                break;
            }
            else {
                state = HEADER;
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
                // end of block
                if(character_code == 256) {
                    break;
                }
                // LZSS code
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
                        tmp_data[j] = image_data.get_component(image_data.size() - match_distance + j);
                    }
                    for(uint32_t j = 0; j < match_length; ++j) {
                        image_data.add_component(tmp_data[j % copy_length]);
                    }
                }
                // raw data
                else {
                    image_data.add_component(static_cast<uint8_t>(character_code));
                }
            }

            if(is_final_block) {
                break;
            }
            else {
                state = HEADER;
            }
        }
        // BTYPE = 0b11 -> invalid
        else {
            throw std::runtime_error("[image::PNG::load] ERROR: unknown compression type found.");
        }
    }


    for(size_t h = 0; h < image_data.filters.size(); ++h) {
        auto row = h * image_data.width * image_data.component_count;
        auto row_minus_one = (h - 1) * image_data.width * image_data.component_count;
        // sub
        if(image_data.filters[h] == 1) {
            for(uint32_t w = 0; w < image_data.width; ++w) {
                auto col = w * image_data.component_count;
                auto col_minus_one = (w - 1) * image_data.component_count;
                for(uint32_t c = 0; c < image_data.component_count; ++c) {
                    auto left = w > 0 ? image_data.data[row + col_minus_one + c] : 0;
                    image_data.data[row + col + c] += left;
                }
            }
        }
        // up
        else if(image_data.filters[h] == 2) {
            for(uint32_t w = 1; w < image_data.width; ++w) {
                auto col = w * image_data.component_count;
                for(uint32_t c = 0; c < image_data.component_count; ++c) {
                    auto up = h > 0 ? image_data.data[row_minus_one + col + c] : 0;
                    image_data.data[row + col + c] += up;
                }
            }  
        }
        // average
        else if(image_data.filters[h] == 3) {
            for(uint32_t w = 0; w < image_data.width; ++w) {
                auto col = w * image_data.component_count;
                auto col_minus_one = (w - 1) * image_data.component_count;
                for(uint32_t c = 0; c < image_data.component_count; ++c) {
                    auto left = w > 0 ? image_data.data[row + col_minus_one + c] : 0;
                    auto up = h > 0 ? image_data.data[row_minus_one + col + c] : 0;
                    image_data.data[row + col + c] += (left + up) / 2;
                }
            }
        }
        // paeth
        else if(image_data.filters[h] == 4) {
            for(uint32_t w = 0; w < image_data.width; ++w) {
                auto col = w * image_data.component_count;
                auto col_minus_one = (w - 1) * image_data.component_count;
                for(uint32_t c = 0; c < image_data.component_count; ++c) {
                    auto left = w > 0 ? image_data.data[row + col_minus_one + c] : 0;
                    auto up = h > 0 ? image_data.data[row_minus_one + col + c] : 0;
                    auto up_left = (w > 0 && h > 0) ? image_data.data[row_minus_one + col_minus_one + c] : 0;
                    image_data.data[row + col + c] += paeth_predictor_(left, up, up_left);
                }
            }
        }
    }

    return PNG(std::move(image_data.data), image_data.width, image_data.height, image_data.component_count);
}

}