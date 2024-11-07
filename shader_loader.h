#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderLoader {
public:
    static std::string loadShaderFromFile(const std::string& filename);
};
