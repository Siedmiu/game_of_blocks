#pragma once

#include "shaders.h"
#include "math.h"
#include "world.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class renderer {
private:
    float timeValue;

    glm::mat4 model;
    unsigned int shaderProgram;

    const world& gameWorld;
    static const unsigned short int CHUNK_HEIGHT = 100, CHUNK_LENGTH = 16;

    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setInt(const std::string& name, int value) const;
    inline int get3dCoord(uint8_t x, uint8_t y, uint8_t z) const {
        return x + CHUNK_LENGTH * (y + CHUNK_LENGTH * z);
    }

public:
    renderer(unsigned int shaderProgram, const world& gameWorld);

    ~renderer() {    };

    void render(unsigned int VAO);
};