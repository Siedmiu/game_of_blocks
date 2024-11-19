// texture_loader.cpp
#include "texture_loader.h"

TextureLoader::TextureLoader() {
    stbi_set_flip_vertically_on_load(true);
}

TextureLoader::~TextureLoader() {}

void TextureLoader::setTextureParameters() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

unsigned int TextureLoader::loadTexture(const std::string& filename) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    setTextureParameters();

    int width, height, nrChannels;
    unsigned char* data = stbi_load(("../textures/" + filename).c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture: " << filename << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}

std::vector<unsigned int> TextureLoader::loadTextures(const std::vector<std::string>& textureFiles) {
    std::vector<unsigned int> textureIDs(textureFiles.size());

    for (size_t i = 0; i < textureFiles.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i + 1);
        textureIDs[i] = loadTexture(textureFiles[i]);
    }

    return textureIDs;
}