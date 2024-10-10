#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class player {
private:
    //CHUNK SETTINGS
    const unsigned short int CHUNK_HEIGHT = 100, CHUNK_LENGTH = 16;
    const float CHUNK_LENGTH_RECIPROCAL = 1.0f / CHUNK_LENGTH;

    float playerX, playerY, playerZ;
    int playerChunkX, playerChunkY;

    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
    };

public:
    player();

    using Aabb = AABB;

    static const unsigned short int PLAYERWIDTH = 1;
    static const unsigned short int PLAYERHEIGHT = 2;

    void updatePlayerPosition(float pX, float pY, float pZ);
    AABB getPlayerAABB() const;
    glm::vec2 getPlayerChunk() const;
    glm::vec3 getPlayerPosition() const;
};