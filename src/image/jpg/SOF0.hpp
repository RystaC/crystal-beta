#pragma once

#include "utils.hpp"

namespace image {

namespace jpg {

struct SOF0Component {
    uint8_t ci;
    uint8_t hi_vi;
    uint8_t tqi;

    uint8_t hi() const noexcept { return (hi_vi & 0xf0) >> 4;}
    uint8_t vi() const noexcept { return hi_vi & 0x0f; }
};

inline SOF0Component read_sof0_component(std::ifstream& ifs) {
    SOF0Component component{};

    read_be(ifs, component.ci);
    read_be(ifs, component.hi_vi);
    read_be(ifs, component.tqi);

    return component;
}

struct SOF0 {
    uint16_t lf;
    uint8_t p;
    uint16_t y;
    uint16_t x;
    std::vector<SOF0Component> components;
};

inline SOF0 read_sof0(std::ifstream& ifs) {
    SOF0 sof0;

    read_be(ifs, sof0.lf);
    read_be(ifs, sof0.p);
    read_be(ifs, sof0.y);
    read_be(ifs, sof0.x);

    uint8_t nf{};
    read_be(ifs, nf);
    sof0.components.resize(nf);
    for(size_t i = 0; i < sof0.components.size(); ++i) {
        sof0.components[i] = std::move(read_sof0_component(ifs));
    }

    return sof0;
}

}

}