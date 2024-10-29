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

    void temporarySetCamera(glm::vec3 set);
    void moveCamera(glm::vec3 move);
    void updateCamera();
    void moveUp(float speed);
    void moveDown(float speed);

    glm::vec2 getXY() const;
    glm::vec3 getXYZ() const;
    glm::mat4 getViewMatrix() const;

    glm::vec3 getCameraPos() const {
        return cameraPos;
    }

    glm::vec3 getCameraFront() const {
        return cameraFront;
    }

    glm::vec3 getCameraFrontXYZ() const {
        return glm::vec3(cameraFront.x, cameraFront.z, cameraFront.y);
    }

    glm::vec3 getCameraUp() const {
        return cameraUp;
    }

    glm::vec3 getCameraUpXYZ() const {
        return glm::vec3(cameraUp.x, cameraUp.z, cameraUp.y);
    }
};