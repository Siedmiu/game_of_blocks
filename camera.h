#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class camera {
private:
    glm::vec3 cameraPos, cameraUp, direction;

public:
    camera();

    bool firstMouse;
    float yaw, pitch, lastX, lastY;
    glm::vec3 cameraFront;

    static camera& getCamera() {
        static camera instance;
        return instance;
    }

    void moveCamera(glm::vec3 move);
    void updateCamera();
    void moveUp(float speed);
    void moveDown(float speed);
    void moveForward(float speed);
    void moveBackward(float speed);
    void moveLeft(float speed);
    void moveRight(float speed);

    glm::vec2 getXY() const;
    glm::vec3 getXYZ() const;
    glm::mat4 getViewMatrix() const;

    glm::vec3 getCameraPos() const {
        return cameraPos;
    }

    glm::vec3 getCameraFront() const {
        return cameraFront;
    }

    glm::vec3 getCameraUp() const {
        return cameraUp;
    }
};