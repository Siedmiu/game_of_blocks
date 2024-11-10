#include "shader_loader.h"

std::string ShaderLoader::loadShaderFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file '" << filename << "'" << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}