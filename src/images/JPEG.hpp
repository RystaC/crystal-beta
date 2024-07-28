#pragma once

#include <iostream>
#include <vector>

namespace images {

namespace jpeg {

namespace markers_ {

constexpr uint16_t DQT = 0xFFDB;
constexpr uint16_t DHT = 0xFFC4;
constexpr uint16_t SOF_baseline = 0xFFC0;
constexpr uint16_t SOF_progressive = 0xFFC2;
constexpr uint16_t SOS = 0xFFDA;

}

namespace tables_ {

inline uint8_t u8_hi(uint8_t value) { return (value & 0xF0) >> 4; }
inline uint8_t u8_lo(uint8_t value) { return (value & 0x0F); }

struct DQT {
    uint16_t Lq;
    uint8_t Pq_Tq;
    union {
        uint8_t Q8[64];
        uint16_t Q16[64];
    } Q;
};

struct DHT {
    uint16_t Lh;
    uint8_t Tcn_Thn;
    uint8_t L[16];
    std::vector<uint8_t> V[16];
};

struct SOF {
    uint16_t Lf;
    uint8_t P;
    uint16_t Y;
    uint16_t X;
    uint8_t Nf;
    uint8_t Cn;
    uint8_t Hn_Vn;
    uint8_t Tqn;
};

struct SOS {
    uint16_t Ls;
    uint8_t Ns;
    uint8_t Csn;
    uint8_t Tdn_Tan;
    uint8_t Ss;
    uint8_t Se;
    uint8_t Ah_Ai;
};

}

}

}