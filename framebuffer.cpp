#include "framebuffer.h"
#include <iostream>

framebuffer::framebuffer(int screenWidth, int screenHeight)
    : SCR_WIDTH(screenWidth), SCR_HEIGHT(screenHeight) {
    setupCanvas();
}

framebuffer::~framebuffer() {
    deleteFramebufferObject(mainBuffer);
    for (auto& buffer : postProcessBuffers) {
        deleteFramebufferObject(buffer);
    }

    glDeleteVertexArrays(1, &canvasVAO);
    glDeleteBuffers(1, &canvasVBO);
}

void framebuffer::setupCanvas() {
    //canvas texture "attatched to the camera"
    glGenVertexArrays(1, &canvasVAO);
    glGenBuffers(1, &canvasVBO);

    glBindVertexArray(canvasVAO);
    glBindBuffer(GL_ARRAY_BUFFER, canvasVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(canvasVertices), canvasVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void framebuffer::createFramebufferObject(framebufferObject& fbo) {
    glGenFramebuffers(1, &fbo.fbo);
    glGenTextures(1, &fbo.texture);
    glGenRenderbuffers(1, &fbo.rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo.fbo);

    glBindTexture(GL_TEXTURE_2D, fbo.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.texture, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, fbo.rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo.rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "FRAMEBUFFER NOT COMPLETE" << std::endl;
    }
}

void framebuffer::deleteFramebufferObject(framebufferObject& fbo) {
    if (fbo.fbo != 0) glDeleteFramebuffers(1, &fbo.fbo);
    if (fbo.texture != 0) glDeleteTextures(1, &fbo.texture);
    if (fbo.rbo != 0) glDeleteRenderbuffers(1, &fbo.rbo);
    fbo = {};
}

void framebuffer::initialize() {
    createFramebufferObject(mainBuffer);
}

void framebuffer::setupPostProcessing(int numberOfPasses) {
    postProcessBuffers.clear();
    postProcessBuffers.resize(numberOfPasses);

    for (auto& buffer : postProcessBuffers) {
        createFramebufferObject(buffer);
    }
}

void framebuffer::bindMainFramebuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, mainBuffer.fbo);
}

void framebuffer::bindPostProcessBuffer(int index) {
    if (index >= 0 && index < postProcessBuffers.size()) {
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessBuffers[index].fbo);
    }
    else {
        std::cerr << "Invalid multipass framebuffer index!" << std::endl;
    }
}

void framebuffer::bindDefaultFramebuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer::renderToScreen(GLuint shader) const {
    glUseProgram(shader);
    glBindVertexArray(canvasVAO);
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mainBuffer.texture);
    glUniform1i(glGetUniformLocation(shader, "screenTexture"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void framebuffer::postProcessingChain(const std::vector<GLuint>& shaders) {
    if (shaders.empty() || postProcessBuffers.empty()) return;

    glBindVertexArray(canvasVAO);
    glDisable(GL_DEPTH_TEST);

    GLuint currentTexture = mainBuffer.texture;

    for (size_t i = 0; i < shaders.size(); ++i) {
        bindPostProcessBuffer(i);

        glUseProgram(shaders[i]);
        glUniform1i(glGetUniformLocation(shaders[i], "inputTexture"), 0);
        glUniform2f(glGetUniformLocation(shaders[i], "screenSize"), static_cast<float>(SCR_WIDTH), static_cast<float>(SCR_HEIGHT));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        currentTexture = postProcessBuffers[i].texture;
    }
}

//true to use the chain, false to just use base framebuffer
void framebuffer::renderFinalOutput(GLuint finalShader, bool usePostProcess) {
    bindDefaultFramebuffer();
    glUseProgram(finalShader);
    glBindVertexArray(canvasVAO);
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    if (usePostProcess && !postProcessBuffers.empty()) {
        glBindTexture(GL_TEXTURE_2D, postProcessBuffers.back().texture);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, mainBuffer.texture);
    }

    glUniform1i(glGetUniformLocation(finalShader, "screenTexture"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

GLuint framebuffer::getPostProcessTexture(int index) const {
    if (index >= 0 && index < postProcessBuffers.size()) {
        return postProcessBuffers[index].texture;
    }
    return 0;
}