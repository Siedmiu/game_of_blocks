// texture_loader.h
#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>
#include <iostream>
#include "stb_image.h"

class TextureLoader {
private:
    void setTextureParameters();

public:
    TextureLoader();
    ~TextureLoader();

    std::vector<unsigned int> loadTextures(const std::vector<std::string>& textureFiles);

    unsigned int loadTexture(const std::string& filename);
};