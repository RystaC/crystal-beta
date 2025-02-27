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

    // SOI
    if(!(marker[0] == 0xff && marker[1] == 0xd8)) {
        throw std::runtime_error(std::format("[image::JPG::load] ERROR: invalid format image: {}", path.string()));
    }

    std::vector<jpg::DQT> dqts{};
    std::vector<jpg::DHT> dhts{};
    jpg::SOF0 sof0{};
    jpg::SOS sos{};

    // read segments
    while(true) {
        jpg::read_be(ifs, marker[0]);
        jpg::read_be(ifs, marker[1]);
        std::cerr << std::format("marker {:02x}{:02x}", marker[0], marker[1]) << std::endl;
        // DQT
        if(marker[0] == 0xff && marker[1] == 0xdb) {
            dqts.emplace_back(std::move(jpg::read_dqt(ifs)));
        }
        // SOF0
        else if(marker[0] == 0xff && marker[1] == 0xc0) {
            sof0 = std::move(jpg::read_sof0(ifs));
        }
        // DHT
        else if(marker[0] == 0xff && marker[1] == 0xc4) {
            dhts.emplace_back(std::move(jpg::read_dht(ifs)));
        }
        // SOS (last segment)
        else if(marker[0] == 0xff && marker[1] == 0xda) {
            sos = std::move(jpg::read_sos(ifs));
            break;
        }
        // other segments
        else if(marker[0] == 0xff) {
            uint16_t length{};
            jpg::read_be(ifs, length);
            ifs.seekg(length - 2, std::ios::cur);
        }
        // invalid
        else {
            throw std::runtime_error(std::format("[image::JPG::load] ERROR: invalid segment"));
        }
    }

    for(size_t i = 0; i < dhts.size(); ++i) {
        std::cerr << std::format("DHT segment #{}", i) << std::endl;
        for(size_t j = 0; j < dhts[i].hts.size(); ++j) {
            std::cerr << std::format("huffman table #{}", j) << std::endl;
            for(size_t k = 0; k < dhts[i].hts[j].li.size(); ++k) {
                std::cerr << std::format("length {}: count = {}", k + 1, dhts[i].hts[j].li[k]) << std::endl;
            }
            for(size_t k = 0; k < dhts[i].hts[j].vi_j.size(); ++k) {
                std::cerr << std::format("value #{}: {:02x}", k, dhts[i].hts[j].vi_j[k]) << std::endl;
            }
        }
    }

    jpg::BitStream ecs{};
    while(true) {
        uint8_t code{};
        ifs.read(reinterpret_cast<char*>(&code), sizeof(uint8_t));

        if(code == 0xff) {
            uint8_t marker{};
            ifs.read(reinterpret_cast<char*>(&marker), sizeof(uint8_t));

            // EOI
            if(marker == 0xd9) {
                break;
            }
            // value
            else if(marker == 0x00) {
                ecs.data.push_back(code);
            }
            // RST
            else if(0xd0 <= marker && marker <= 0xd7) {
                continue;
            }
            // invalid
            else {
                throw std::runtime_error(std::format("[image::JPG::load] ERROR: invalid code: {:x}{:x}", code, marker));
            }
        }
        else {
            ecs.data.push_back(code);
        }
    }

    std::cerr << std::format("{}", ecs.size()) << std::endl;

    return {};
}

}