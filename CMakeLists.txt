cmake_minimum_required(VERSION 3.5.0)

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
file(GLOB_RECURSE sources
    "src/*.cpp"
    "src/*.hpp"
    "src/*.h"
)

include_directories(app ${ADDITIONAL_INCLUDE_DIRECTORIES})
add_executable(app ${sources})
target_compile_definitions(app PRIVATE _CRT_SECURE_NO_WARNINGS)
target_include_directories(app PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(app ${SDL2_LIBRARIES})
target_include_directories(app PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(app ${Vulkan_LIBRARIES})
# change subsystem to console (maybe debug build only)
set_target_properties(app PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
# suppress warnings
set_target_properties(app PROPERTIES LINK_FLAGS "/NODEFAULTLIB:library")
set_target_properties(app PROPERTIES LINK_FLAGS "/IGNORE:4099")

# add_executable(server network/server.cpp)

# add_executable(client network/client.cpp)

# shader compile
set(glslc "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")

file(GLOB_RECURSE shader_sources
    "src/glsl/*.glsl"
)

foreach(shader ${shader_sources})
    get_filename_component(file_name ${shader} NAME)
    set(spirv "${PROJECT_BINARY_DIR}/spirv/${file_name}.spirv")
    add_custom_command(
        OUTPUT ${spirv}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/spirv"
        COMMAND ${glslc} ${shader} -V --target-env spirv1.3 -o ${spirv}
        DEPENDS ${shader}
    )
    list(APPEND spirv_binaries ${spirv})
endforeach(shader ${shader_sources})

add_custom_target(shaders DEPENDS ${spirv_binaries})
add_dependencies(app shaders)