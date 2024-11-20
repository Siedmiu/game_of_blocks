#include <iostream>

#include "framebuffer.h"

framebuffer::framebuffer(int SCR_WIDTH, int SCR_HEIGHT)
    :SCR_WIDTH(SCR_WIDTH), SCR_HEIGHT(SCR_HEIGHT) {
    
    glGenFramebuffers(1, &FBO);
    glGenRenderbuffers(1, &RBO);

    glGenVertexArrays(1, &framebufferVAO);
    glGenBuffers(1, &framebufferVBO);

    glGenTextures(NUMBER_OF_FRAMEBUFFERTEXTURES, framebufferTexture);
}

//this will now only be one, the rest is in multipasses
void framebuffer::createFramebuffer() {
    //frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    //frame buffer textures
    for (int i = 0; i < NUMBER_OF_FRAMEBUFFERTEXTURES; i++) createTextures(i);

    //render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    //VAO VBO
    glBindVertexArray(framebufferVAO);
    glBindBuffer(GL_ARRAY_BUFFER, framebufferVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(framebufferVertices), &framebufferVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FRAMEBUFFER NOT COMPLETE" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer::createTextures(int index) {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture[index]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, framebufferTexture[index], 0);
}

int framebuffer::getNumberOfTextures() const {
    return NUMBER_OF_FRAMEBUFFERTEXTURES;
}

void framebuffer::bindFramebuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void framebuffer::unbindFramebuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer::postProcess(unsigned int framebufferShaderProgram) const {
    glUseProgram(framebufferShaderProgram);
    glBindVertexArray(framebufferVAO);
    glDisable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D, framebufferTexture[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

//multi pass
void framebuffer::createMultiPassFramebuffers(int numberOfPasses) {
    multiPassFBOs.clear();
    multiPassTextures.clear();
    multiPassFBOs.resize(numberOfPasses);
    multiPassTextures.resize(numberOfPasses);

    glGenFramebuffers(numberOfPasses, multiPassFBOs.data());
    glGenTextures(numberOfPasses, multiPassTextures.data());

    for (int i = 0; i < numberOfPasses; ++i) { //to usunac
        glBindFramebuffer(GL_FRAMEBUFFER, multiPassFBOs[i]);

        //nowa textura, potem usunac wczesniejsza wersje
        glBindTexture(GL_TEXTURE_2D, multiPassTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, multiPassTextures[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "MULTIPASS FRAMEBUFFER" << i << " NOT COMPLETE" << std::endl;
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer::bindMultiPassFramebuffer(int framebufferPassIndex) {
    if (framebufferPassIndex >= 0 && framebufferPassIndex < multiPassFBOs.size()) {
        glBindFramebuffer(GL_FRAMEBUFFER, multiPassFBOs[framebufferPassIndex]);
    }
    else {
        std::cerr << "Invalid multipass framebuffer index!" << std::endl;
    } //to usunac
}

GLuint framebuffer::getMultiPassTexture(int framebufferPassIndex) const {
    if (framebufferPassIndex >= 0 && framebufferPassIndex < multiPassTextures.size()) {
        return multiPassTextures[framebufferPassIndex];
    }
    return 0;
}

void framebuffer::postProcessingChain(const std::vector<unsigned int>& shaderPrograms) {
    if (shaderPrograms.empty()) return;
    glBindVertexArray(framebufferVAO);
    glDisable(GL_DEPTH_TEST);

    //first pass uses original framebuffertexture
    GLuint currentTexture = framebufferTexture[0];

    for (size_t i = 0; i < shaderPrograms.size(); ++i) {
        bindMultiPassFramebuffer(i);
        glUseProgram(shaderPrograms[i]);
        glUniform1i(glGetUniformLocation(shaderPrograms[i], "inputTexture"), 0);
        glUniform2f(glGetUniformLocation(shaderPrograms[i], "screenSize"), static_cast<float>(SCR_WIDTH), static_cast<float>(SCR_HEIGHT));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        currentTexture = multiPassTextures[i];
    }

    //framebuffer pass render
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(shaderPrograms.back());
    glUniform1i(glGetUniformLocation(shaderPrograms.back(), "inputTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, currentTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

framebuffer::~framebuffer() {
    glDeleteVertexArrays(1, &framebufferVAO);
    glDeleteBuffers(1, &framebufferVBO);
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);
    glDeleteTextures(NUMBER_OF_FRAMEBUFFERTEXTURES, framebufferTexture);

    //multipass cleanup
    if (!multiPassFBOs.empty()) {
        glDeleteFramebuffers(multiPassFBOs.size(), multiPassFBOs.data());
    }
    if (!multiPassTextures.empty()) {
        glDeleteTextures(multiPassTextures.size(), multiPassTextures.data());
    }
}