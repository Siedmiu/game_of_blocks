#pragma once

#include <glad/glad.h>
#include <vector>

class framebuffer {
private:
    void createTextures(int index);
    int SCR_WIDTH, SCR_HEIGHT;
    const int NUMBER_OF_FRAMEBUFFERTEXTURES = 1;

    unsigned int FBO{}, RBO{};
    unsigned int framebufferVAO{}, framebufferVBO{};

    unsigned int framebufferTexture[1];

    //CCW winding order
    const float framebufferVertices[24] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    //multi pass
    std::vector<GLuint> multiPassFBOs;
    std::vector<GLuint> multiPassTextures;

public:
    framebuffer(int SCR_WIDTH, int SCR_HEIGHT);
    ~framebuffer();

    //framebuffer setup
    int getNumberOfTextures() const;
    void createFramebuffer();
    void bindFramebuffer() const;
    void unbindFramebuffer() const;
    void postProcess(unsigned int framebufferShaderProgram) const;

    //multi pass
    void createMultiPassFramebuffers(int numberOfPasses);
    void bindMultiPassFramebuffer(int framebufferPassIndex);
    GLuint getMultiPassTexture(int framebufferPassIndex) const;
    void postProcessingChain(const std::vector<unsigned int>& shaderPrograms);
};