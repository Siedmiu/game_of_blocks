#pragma once

#include "camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class player {
private:
    const float WALKING_ACCELERATION = 1, SPRINTING_ACCELERATION = 2, AIR_RESISTANCE = 0.5, FLOOR_FRICTION = 0.5;
    const float MAX_WALKING_SPEED = 4.3, MAX_SPRINT_SPEED = 7.1;
    const float JUMP_SPEED = 10;

    float playerX, playerY, playerZ;
    glm::vec3 playerVelocity;
    glm::vec3 cameraFront, cameraUp;
    int playerChunkX, playerChunkY;

    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
    };
    camera& cam;

    void decelerate(float time);

public:
    player(camera& cam);

    using Aabb = AABB;

    static const unsigned short int PLAYERWIDTH = 1;
    static const unsigned short int PLAYERHEIGHT = 2;

    //void accelUP(float time);
    void accelFRONTwalk(float time);
    void accelFRONTsprint(float time);
    void accelBACK(float time);
    void accelLEFT(float time);
    void accelRIGHT(float time);

    void movePlayer(float time);

    void updateCameraFront();
    void updateCameraUp();
    void updatePlayerPosition(float pX, float pY, float pZ);
    void updatePlayerVelocity(float vX, float vY, float vZ);
    AABB getPlayerAABB() const;
    //glm::vec2 getPlayerChunk() const;
    glm::vec3 getPlayerPosition() const;
    glm::vec3 getPlayerVelocity() const;
};