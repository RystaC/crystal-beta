#pragma once

#include <filesystem>
#include <fstream>

namespace meshes {

class Obj {

public:
    static void load(const std::filesystem::path& path);
};

}