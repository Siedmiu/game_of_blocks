#pragma once

#include "camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class player {
private:
    //they dont all need to be floats
    const float WALKING_ACCELERATION = 20.0f, SPRINTING_ACCELERATION = 40.0f, AIR_RESISTANCE = 10.0f, FLOOR_FRICTION = 10.0f;
    const float MAX_WALKING_SPEED = 4.3f, MAX_SPRINT_SPEED = 7.1f;
    const float JUMP_SPEED = 10.0f;

    float playerX, playerY, playerZ;
    glm::vec3 playerVelocity;
    glm::vec3 cameraFrontXYZ, cameraUpXYZ, forward;
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
    void accelFlyUP(float time);
    void accelFlyDOWN(float time);

    void movePlayer(float time);

    void updatePlayerPosition(float pX, float pY, float pZ);
    void updatePlayerPositionVector(glm::vec3 pos);
    void updatePlayerVelocity(float vX, float vY, float vZ);
    AABB getPlayerAABB() const;
    AABB getPlayerBroadAABB() const;
    //glm::vec2 getPlayerChunk() const;
    glm::vec3 getPlayerPosition() const;
    glm::vec3 getPlayerVelocity() const;
};