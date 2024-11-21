#pragma once
#include <glad/glad.h>
#include <vector>
#include <memory>

class framebuffer {
private:
    struct framebufferObject {
        GLuint fbo = 0;
        GLuint texture = 0;
        GLuint rbo = 0;
    };

    const int SCR_WIDTH;
    const int SCR_HEIGHT;

    //old framebuffer VAO/VBO
    GLuint canvasVAO = 0;
    GLuint canvasVBO = 0;
    static constexpr float canvasVertices[24] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    //base framebuffer
    framebufferObject mainBuffer;

    //multichain
    std::vector<framebufferObject> postProcessBuffers;

    void setupCanvas();
    //to powinno byc prywante!
    void createFramebufferObject(framebufferObject& fbo);
    void deleteFramebufferObject(framebufferObject& fbo);

public:
    framebuffer(int screenWidth, int screenHeight);
    ~framebuffer();

    //
    framebuffer(const framebuffer&) = delete;
    framebuffer& operator=(const framebuffer&) = delete;

    void initialize();
    void setupPostProcessing(int numberOfPasses);

    void bindMainFramebuffer() const;
    void bindPostProcessBuffer(int index);
    void bindDefaultFramebuffer() const;

    //post processing methods
    void renderToScreen(GLuint shader) const;
    void postProcessingChain(const std::vector<GLuint>& shaders);
    void renderFinalOutput(GLuint finalShader, bool usePostProcess = true);
    void cannyOverlay(const std::vector<GLuint>& cannyShaders, const std::vector<GLuint>& kuwaharaShaders, GLuint overlayShader);

    GLuint getMainColorTexture() const { return mainBuffer.texture; }
    GLuint getPostProcessTexture(int index) const;
};