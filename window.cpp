#include "window.h"

const float STATIC_SPEED = 5.5f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    camera &cam = camera::getCamera();

    if (cam.firstMouse)
    {
        cam.lastX = xpos;
        cam.lastY = ypos;
        cam.firstMouse = false;
    }

    float xoffset = xpos - cam.lastX;
    float yoffset = cam.lastY - ypos;
    cam.lastX = xpos;
    cam.lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cam.yaw += xoffset;
    cam.pitch += yoffset;

    // ograniczenia katow
    if (cam.pitch > 89.0f)
        cam.pitch = 89.0f;
    if (cam.pitch < -89.0f)
        cam.pitch = -89.0f;

    cam.updateCamera();
}

void processInput(GLFWwindow* window, float TIMESTEP, player& player) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        //return false;
    } 

    //const float cameraSpeed = STATIC_SPEED * TIMESTEP;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        player.accelFRONTwalk(TIMESTEP);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        player.accelBACK(TIMESTEP);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        player.accelLEFT(TIMESTEP);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        player.accelRIGHT(TIMESTEP);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        //cam.moveUp(cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        //cam.moveDown(cameraSpeed);
    }
}