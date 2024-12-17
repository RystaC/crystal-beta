#include "TrueType.hpp"

namespace font {

TrueType TrueType::load(const std::filesystem::path& path) {
    TrueType tt{};

    std::ifstream ifs(path, std::ios::in | std::ios::binary);

    if(ifs.fail()) {
        throw std::runtime_error(std::format("[font::TrueType::load] ERROR: failed to read file: {}", path.string().c_str()));
    }

    uint32_t scaler_type{};
    uint16_t table_count{};
    uint16_t search_range{};
    uint16_t entry_selector{};
    uint16_t range_shift{};
    read_be_(ifs, scaler_type);
    read_be_(ifs, table_count);
    read_be_(ifs, search_range);
    read_be_(ifs, entry_selector);
    read_be_(ifs, range_shift);

    std::unordered_map<std::string, TableDirectory_> tables{};

    for(uint32_t i = 0; i < table_count; ++i) {
        auto [tag, td] = read_table_directory_(ifs);
        tables[tag] = td;
    }

    // required tables
    // cmap
    // glyf
    // head
    // hhea
    // hmtx
    // loca
    // maxp
    // name
    // post

    // optional header
    // cvt
    // fpgm
    // hdmx
    // kern
    // OS/2
    // prep

    ifs.seekg(tables["head"].offset, std::ios::beg);
    auto head = true_type::read_head(ifs);
    tt.head_ = head;
    std::cerr << std::format("global bounding box = min({}, {}), max({}, {})", head.x_min, head.y_min, head.x_max, head.y_max) << std::endl;

    ifs.seekg(tables["maxp"].offset, std::ios::beg);
    auto maxp = true_type::read_maxp(ifs);
    tt.maxp_ = maxp;

    ifs.seekg(tables["loca"].offset, std::ios::beg);
    true_type::Loca loca{};
    if(head.index_to_loc_format == 0) {
        loca = std::move(true_type::read_loca_short(ifs, maxp.num_glyphs));
    }
    else {
        loca = std::move(true_type::read_loca_long(ifs, maxp.num_glyphs));
    }

    ifs.seekg(tables["cmap"].offset, std::ios::beg);
    auto cmap = true_type::read_cmap(ifs);

    ifs.seekg(tables["cmap"].offset + cmap.subtables[2].offset, std::ios::beg);
    uint16_t format{};
    read_be_(ifs, format);
    auto cmap12 = true_type::cmap::read_format12(ifs);
    tt.char_map_ = cmap12;

    std::vector<true_type::Glyf> glyphs(maxp.num_glyphs);
    for(size_t i = 0; i < glyphs.size(); ++i) {
        ifs.seekg(tables["glyf"].offset + loca.offsets[i], std::ios::beg);
        glyphs[i] = std::move(true_type::read_glyf(ifs));
    }
    tt.glyphs_ = glyphs;

    return tt;
}

}