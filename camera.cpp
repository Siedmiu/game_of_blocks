#include "camera.h"

camera::camera() {
    cameraPos = glm::vec3(0.0f, 100.0f, 3.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    lastX = 400; //dlaczego tu tak jest nie pamietal lol
    lastY = 300;
    yaw = -90.0f;
    firstMouse = true;

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
}

void camera::temporarySetCamera(glm::vec3 set) {
    cameraPos = set;
}

void camera::moveCamera(glm::vec3 move) {
    cameraPos += move;
}

void camera::updateCamera() {
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void camera::moveUp(float speed) {
    cameraPos += speed * cameraUp;
}

void camera::moveDown(float speed) {
    cameraPos -= speed * cameraUp;
}

glm::vec2 camera::getXY() const {
    return glm::vec2(cameraPos.x, cameraPos.z);
}

glm::vec3 camera::getXYZ() const {
    return glm::vec3(cameraPos.x, cameraPos.z, cameraPos.y);
}

glm::mat4 camera::getViewMatrix() const {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}