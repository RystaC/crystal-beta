#pragma once

#include "utils.hpp"

namespace image {

namespace jpg {

struct DHT {
    uint16_t lh;
    struct HT {
        uint8_t tc_th;
        std::array<uint8_t, 16> li;
        std::vector<uint8_t> vi_j;

        uint8_t tc() const noexcept { return (tc_th & 0xf0) >> 4; }
        uint8_t th() const noexcept { return tc_th & 0x0f; }
    };

    std::vector<HT> hts;
};

inline DHT read_dht(std::ifstream& ifs) {
    DHT dht{};

    read_be(ifs, dht.lh);
    
    auto table_size = dht.lh - 2;
    while(table_size > 0) {
        DHT::HT ht{};

        read_be(ifs, ht.tc_th);
        table_size -= 1;
        for(size_t i = 0; i < ht.li.size(); ++i) {
            read_be(ifs, ht.li[i]);
        }
        table_size -= 16;
        
        ht.vi_j.resize(std::accumulate(ht.li.begin(), ht.li.end(), 0));
        for(size_t i = 0; i < ht.vi_j.size(); ++i) {
            read_be(ifs, ht.vi_j[i]);
        }
        table_size -= sizeof(uint8_t) * static_cast<int>(ht.vi_j.size());

        dht.hts.emplace_back(std::move(ht));
    }

    return dht;
}

}

}