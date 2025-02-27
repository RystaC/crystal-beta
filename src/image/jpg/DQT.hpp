#pragma once

#include "utils.hpp"

namespace image {

namespace jpg {

struct DQT {
    uint16_t lq;
    
    struct QT {
        uint8_t pq_tq;

        union QK {
            std::array<uint8_t, 64> u8;
            std::array<uint16_t, 64> u16; 
        } qk;

        uint8_t pq() const noexcept { return (pq_tq & 0xf0) >> 4; }
        uint8_t tq() const noexcept { return pq_tq & 0x0f; }
    };
    std::vector<QT> qts;

};

inline DQT read_dqt(std::ifstream& ifs) {
    DQT dqt{};

    read_be(ifs, dqt.lq);

    auto table_size = dqt.lq - 2;
    while(table_size > 0) {
        DQT::QT qt{};
        read_be(ifs, qt.pq_tq);
        table_size -= 1;
        // 8bit table
        if(qt.pq() == 0) {
            for(size_t i = 0; i < qt.qk.u8.size(); ++i) {
                read_be(ifs, qt.qk.u8[i]);
            }
            table_size -= 64;
        }
        // 16bit table
        else {
            for(size_t i = 0; i < qt.qk.u16.size(); ++i) {
                read_be(ifs, qt.qk.u16[i]);
            }
            table_size -= 128;
        }

        dqt.qts.emplace_back(std::move(qt));
    }

    return dqt;
}

}

}