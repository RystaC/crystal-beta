#pragma once

#include <filesystem>
#include <fstream>

namespace mesh {

class Obj {

public:
    static void load(const std::filesystem::path& path);
};

}