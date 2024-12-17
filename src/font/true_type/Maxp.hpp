#pragma once

#include "utils.hpp"

namespace font {

namespace true_type {

struct Maxp {
    Fixed version;
    uint16_t num_glyphs;
    uint16_t max_points;
    uint16_t max_contours;
    uint16_t max_component_points;
    uint16_t max_component_contours;
    uint16_t max_zones;
    uint16_t max_twilight_points;
    uint16_t max_storages;
    uint16_t max_function_defs;
    uint16_t max_instruction_defs;
    uint16_t max_stack_elements;
    uint16_t max_size_of_instructions;
    uint16_t max_component_elements;
    uint16_t max_component_depth;
};

inline Maxp read_maxp(std::ifstream& ifs) {
    Maxp maxp{};

    read_be(ifs, maxp.version);
    read_be(ifs, maxp.num_glyphs);
    read_be(ifs, maxp.max_points);
    read_be(ifs, maxp.max_contours);
    read_be(ifs, maxp.max_component_points);
    read_be(ifs, maxp.max_component_contours);
    read_be(ifs, maxp.max_zones);
    read_be(ifs, maxp.max_twilight_points);
    read_be(ifs, maxp.max_storages);
    read_be(ifs, maxp.max_function_defs);
    read_be(ifs, maxp.max_instruction_defs);
    read_be(ifs, maxp.max_stack_elements);
    read_be(ifs, maxp.max_size_of_instructions);
    read_be(ifs, maxp.max_component_elements);
    read_be(ifs, maxp.max_component_depth);

    return maxp;
}

}

}