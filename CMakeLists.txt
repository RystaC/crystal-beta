cmake_minimum_required(VERSION 3.1.0)

project(crystal_beta)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
include(CheckCXXCompilerFlag)

# c++ settings
enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# find Vulkan
set(Vulkan_INCLUDE_DIRS "$ENV{VULKAN_SDK}/Include")
set(Vulkan_LIBRARIES "$ENV{VULKAN_SDK}/Lib")
find_package(Vulkan REQUIRED)

# find SDL2
set(SDL2_DIR "$ENV{VULKAN_SDK}/cmake")
find_package(SDL2 CONFIG REQUIRED)

# collect source files
# TODO: use subdirectories
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.hpp"
    "src/*.h"
)

include_directories(app ${ADDITIONAL_INCLUDE_DIRECTORIES})

add_executable(app ${SOURCES})

target_include_directories(app PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(app ${SDL2_LIBRARIES})
target_include_directories(app PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(app ${Vulkan_LIBRARIES})

# change subsystem to console (maybe debug build only)
set_target_properties(app PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
# suppress warnings
set_target_properties(app PROPERTIES LINK_FLAGS "/NODEFAULTLIB:library")
set_target_properties(app PROPERTIES LINK_FLAGS "/IGNORE:4099")