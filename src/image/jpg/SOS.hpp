#pragma once

#include "utils.hpp"

namespace image {

namespace jpg {

struct SOSComponent {
    uint8_t csj;
    uint8_t tdj_taj;

    uint8_t tdj() const noexcept { return (tdj_taj & 0xf0) >> 4; }
    uint8_t taj() const noexcept { return tdj_taj & 0x0f; }
};

inline SOSComponent read_sos_component(std::ifstream& ifs) {
    SOSComponent component{};

    read_be(ifs, component.csj);
    read_be(ifs, component.tdj_taj);

    return component;
}

struct SOS {
    uint16_t ls;
    std::vector<SOSComponent> components;
    uint8_t ss;
    uint8_t se;
    uint8_t ah_al;

    uint8_t ah() const noexcept { return (ah_al & 0xf0) >> 4; }
    uint8_t al() const noexcept { return ah_al & 0x0f; }
};

inline SOS read_sos(std::ifstream& ifs) {
    SOS sos{};

    read_be(ifs, sos.ls);

    uint8_t ns{};
    read_be(ifs, ns);
    sos.components.resize(ns);
    for(size_t i = 0; i < sos.components.size(); ++i) {
        sos.components[i] = std::move(read_sos_component(ifs));
    }

    read_be(ifs, sos.ss);
    read_be(ifs, sos.se);
    read_be(ifs, sos.ah_al);

    return sos;
}

}

}