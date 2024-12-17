#pragma once

#include "cmap/Subtable.hpp"
#include "cmap/Format0.hpp"
#include "cmap/Format2.hpp"
#include "cmap/Format4.hpp"
#include "cmap/Format12.hpp"

namespace font {

namespace true_type {

struct Cmap {
    uint16_t version;
    std::vector<cmap::Subtable> subtables;
};

inline Cmap read_cmap(std::ifstream& ifs) {
    Cmap cm{};

    read_be(ifs, cm.version);
    uint16_t subtable_count{};
    read_be(ifs, subtable_count);
    cm.subtables.resize(subtable_count);
    for(size_t i = 0; i < cm.subtables.size(); ++i) {
        cm.subtables[i] = std::move(cmap::read_subtable(ifs));
    }

    return cm;
}

}

}