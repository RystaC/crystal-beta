#pragma once

#include "utils.hpp"

namespace font {

namespace true_type {

struct Glyf {
    int16_t num_contours;
    int16_t x_min, y_min, x_max, y_max;
    std::vector<uint16_t> end_pts_of_contours;
    std::vector<uint8_t> instructions;
    std::vector<uint8_t> flags;
    std::vector<int16_t> x_coordinates;
    std::vector<int16_t> y_coordinates;
};

inline Glyf read_glyf(std::ifstream& ifs) {
    Glyf glyf{};

    read_be(ifs, glyf.num_contours);
    read_be(ifs, glyf.x_min);
    read_be(ifs, glyf.y_min);
    read_be(ifs, glyf.x_max);
    read_be(ifs, glyf.y_max);

    glyf.end_pts_of_contours.resize(glyf.num_contours);
    for(size_t i = 0; i < glyf.end_pts_of_contours.size(); ++i) {
        read_be(ifs, glyf.end_pts_of_contours[i]);
    }

    uint16_t instruction_length{};
    read_be(ifs, instruction_length);
    glyf.instructions.resize(instruction_length);
    for(size_t i = 0; i < glyf.instructions.size(); ++i) {
        read_be(ifs, glyf.instructions[i]);
    }

    glyf.flags.resize(glyf.end_pts_of_contours.back() + 1);
    glyf.x_coordinates.resize(glyf.end_pts_of_contours.back() + 1);
    glyf.y_coordinates.resize(glyf.end_pts_of_contours.back() + 1);

    for(size_t i = 0; i < glyf.flags.size(); ++i) {
        uint8_t flag{};
        read_be(ifs, flag);
        glyf.flags[i] = flag;
        if(flag & 0x08) {
            uint8_t count{};
            read_be(ifs, count);
            for(uint8_t c = 0; c < count; ++c) {
                glyf.flags[++i] = flag;
            }
        }
    }

    for(size_t i = 0; i < glyf.x_coordinates.size(); ++i) {
        if(glyf.flags[i] & 0x02) {
            uint8_t x{};
            read_be(ifs, x);
            if(glyf.flags[i] & 0x10) { 
                if(i == 0) {
                    glyf.x_coordinates[i] = static_cast<int16_t>(x);
                }
                else {
                    glyf.x_coordinates[i] = glyf.x_coordinates[i-1] + static_cast<int16_t>(x);
                }
            }
            else {
                if(i == 0) {
                    glyf.x_coordinates[i] = -static_cast<int16_t>(x);
                }
                else {
                    glyf.x_coordinates[i] = glyf.x_coordinates[i-1] - static_cast<int16_t>(x);
                }
            }
        }
        else {
            if(glyf.flags[i] & 0x10) {
                if(i == 0) {
                    glyf.x_coordinates[i] = 0;
                }
                else {
                    glyf.x_coordinates[i] = glyf.x_coordinates[i-1];
                }
            }
            else {
                int16_t x{};
                read_be(ifs, x);
                if(i == 0) {
                    glyf.x_coordinates[i] = x;
                }
                else {
                    glyf.x_coordinates[i] = glyf.x_coordinates[i-1] + x;
                }
            }
        }
    }

    for(size_t i = 0; i < glyf.y_coordinates.size(); ++i) {
        if(glyf.flags[i] & 0x04) {
            uint8_t y{};
            read_be(ifs, y);
            if(glyf.flags[i] & 0x20) { 
                if(i == 0) {
                    glyf.y_coordinates[i] = static_cast<int16_t>(y);
                }
                else {
                    glyf.y_coordinates[i] = glyf.y_coordinates[i-1] + static_cast<int16_t>(y);
                }
            }
            else {
                if(i == 0) {
                    glyf.y_coordinates[i] = -static_cast<int16_t>(y);
                }
                else {
                    glyf.y_coordinates[i] = glyf.y_coordinates[i-1] - static_cast<int16_t>(y);
                }
            }
        }
        else {
            if(glyf.flags[i] & 0x20) {
                if(i == 0) {
                    glyf.y_coordinates[i] = 0;
                }
                else {
                    glyf.y_coordinates[i] = glyf.y_coordinates[i-1];
                }
            }
            else {
                int16_t y{};
                read_be(ifs, y);
                if(i == 0) {
                    glyf.y_coordinates[i] = y;
                }
                else {
                    glyf.y_coordinates[i] = glyf.y_coordinates[i-1] + y;
                }
            }
        }
    }

    return glyf;
}

}

}