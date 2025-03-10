#include "player.h"

player::player(camera& cam) : cam(cam), forward(glm::vec3(0.0f, 0.0f, 0.0f)), cameraFrontXYZ(glm::vec3(0.0f, -1.0f, 0.0f)), cameraUpXYZ(glm::vec3(0.0f, 0.0f, 1.0f)) {
    playerX = 0;
	playerY = 0;
	playerZ = 0;
	playerVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
	playerChunkX = 0;
	playerChunkY = 0;
}

//this shouldnt be affected by falling
void player::accelFRONTwalk(float time) {
    float currentSpeed = glm::dot(playerVelocity, forward);

    if (currentSpeed < MAX_WALKING_SPEED) {
        playerVelocity += forward * WALKING_ACCELERATION * time;

        float newSpeed = glm::length(playerVelocity);
        if (newSpeed > MAX_WALKING_SPEED) {
            playerVelocity = glm::normalize(playerVelocity) * MAX_WALKING_SPEED;
        }
    }
}

void player::accelFRONTsprint(float time) {
    float currentSpeed = glm::dot(playerVelocity, forward);

    if (currentSpeed < MAX_SPRINT_SPEED) {
        playerVelocity += forward * SPRINTING_ACCELERATION * time;

        float newSpeed = glm::length(playerVelocity);
        if (newSpeed > MAX_SPRINT_SPEED) {
            playerVelocity = glm::normalize(playerVelocity) * MAX_SPRINT_SPEED;
        }
    }
}

void player::accelBACK(float time) {
    float currentSpeed = glm::dot(playerVelocity, -forward);

    if (currentSpeed < MAX_WALKING_SPEED) {
        playerVelocity += -forward * WALKING_ACCELERATION * time;

        float newSpeed = glm::length(playerVelocity);
        if (newSpeed > MAX_WALKING_SPEED) {
            playerVelocity = glm::normalize(playerVelocity) * MAX_WALKING_SPEED;
        }
    }
}

void player::accelLEFT(float time) {
    glm::vec3 left = glm::normalize(glm::cross(cameraFrontXYZ, cameraUpXYZ));
    float currentSpeed = glm::dot(playerVelocity, left);

    if (currentSpeed < MAX_WALKING_SPEED) {
        playerVelocity += left * WALKING_ACCELERATION * time;

        float newSpeed = glm::length(playerVelocity);
        if (newSpeed > MAX_WALKING_SPEED) {
            playerVelocity = glm::normalize(playerVelocity) * MAX_WALKING_SPEED;
        }
    }
}

void player::accelRIGHT(float time) {
    glm::vec3 right = -glm::normalize(glm::cross(cameraFrontXYZ, cameraUpXYZ));
    float currentSpeed = glm::dot(playerVelocity, right);

    if (currentSpeed < MAX_WALKING_SPEED) {
        playerVelocity += right * WALKING_ACCELERATION * time;

        float newSpeed = glm::length(playerVelocity);
        if (newSpeed > MAX_WALKING_SPEED) {
            playerVelocity = glm::normalize(playerVelocity) * MAX_WALKING_SPEED;
        }
    }
}

//ADD FLY ABLE STATE
void player::accelFlyUP(float time) {
    glm::vec3 up = cameraUpXYZ;

    float currentSpeed = glm::dot(playerVelocity, up);

    if (currentSpeed < MAX_WALKING_SPEED) {
        playerVelocity += up * WALKING_ACCELERATION * time;

        float newSpeed = glm::length(playerVelocity);
        if (newSpeed > MAX_WALKING_SPEED) {
            playerVelocity = glm::normalize(playerVelocity) * MAX_WALKING_SPEED;
        }
    }
}

void player::accelFlyDOWN(float time) {
    glm::vec3 up = cameraUpXYZ;
    float currentSpeed = glm::dot(playerVelocity, -up);

    if (currentSpeed < MAX_WALKING_SPEED) {
        playerVelocity += -up * WALKING_ACCELERATION * time;

        float newSpeed = glm::length(playerVelocity);
        if (newSpeed > MAX_WALKING_SPEED) {
            playerVelocity = glm::normalize(playerVelocity) * MAX_WALKING_SPEED;
        }
    }
}


void player::decelerate(float time) {
    //AIR AND FLOOR
    float speed = glm::length(playerVelocity);
    if (speed > 0) {
        float decel = AIR_RESISTANCE * time;
        if (speed < decel) {
            playerVelocity = glm::vec3(0.0f);
        }
        else {
            playerVelocity -= glm::normalize(playerVelocity) * decel;
        }
    }
}

void player::movePlayer(float time) {
    //move with collision check instead
    playerX += playerVelocity.x * time;
    playerY += playerVelocity.y * time;
    playerZ += playerVelocity.z * time;

    //air resistance and gravity
    decelerate(time);



    //update forward
    cameraFrontXYZ = cam.getCameraFrontXYZ();
    cameraUpXYZ = cam.getCameraUpXYZ();
    forward = glm::normalize(glm::vec3(cameraFrontXYZ.x, cameraFrontXYZ.y, 0.0f));

    //move camera (temp set)
    cam.temporarySetCamera(glm::vec3(playerX,playerZ,playerY));
}

void player::updatePlayerPosition(float pX, float pY, float pZ) {
	playerX = pX;
	playerY = pY;
	playerZ = pZ;
}

void player::updatePlayerPositionVector(glm::vec3 pos) {
    playerX = pos.x;
    playerY = pos.y;
    playerZ = pos.z;
}

void player::updatePlayerVelocity(float vX, float vY, float vZ) {
	playerVelocity = glm::vec3(vX, vY, vZ);
}

player::AABB player::getPlayerAABB() const {
	return {
		glm::vec3(playerX - PLAYERWIDTH / 2, playerY - PLAYERWIDTH / 2, playerZ - PLAYERHEIGHT),
		glm::vec3(playerX + PLAYERWIDTH / 2, playerY + PLAYERWIDTH / 2, playerZ)
	};
}

player::AABB player::getPlayerBroadAABB() const {
    AABB broadAABB{};

    //player AABB in brackets ()
    if (playerVelocity.x > 0) {
        broadAABB.min.x = (playerX - PLAYERWIDTH / 2);
        broadAABB.max.x = (playerX + PLAYERWIDTH / 2) + playerVelocity.x;
    }
    else {
        broadAABB.min.x = (playerX - PLAYERWIDTH / 2) + playerVelocity.x;
        broadAABB.max.x = (playerX + PLAYERWIDTH / 2);
    };

    if (playerVelocity.y > 0) {
        broadAABB.min.y = (playerY - PLAYERWIDTH / 2);
        broadAABB.max.y = (playerY + PLAYERWIDTH / 2) + playerVelocity.y;
    }
    else {
        broadAABB.min.y = (playerY - PLAYERWIDTH / 2) + playerVelocity.y;
        broadAABB.max.y = (playerY + PLAYERWIDTH / 2);
    };

    if (playerVelocity.z > 0) {
        broadAABB.min.z = (playerZ - PLAYERHEIGHT);
        broadAABB.max.z = (playerZ) + playerVelocity.z;
    } else {
        broadAABB.min.z = (playerZ - PLAYERHEIGHT) + playerVelocity.z;
        broadAABB.max.z = (playerZ);
    };

    return broadAABB;
}

/*
glm::vec2 player::getPlayerChunk() const {
	return glm::vec2(playerChunkX, playerChunkY);
}*/

glm::vec3 player::getPlayerPosition() const {
	return glm::vec3(playerX, playerY, playerZ);
}

glm::vec3 player::getPlayerVelocity() const {
	return playerVelocity;
}