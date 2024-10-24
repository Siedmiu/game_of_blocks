#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "player.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window, float TIMESTEP, camera& cam, player& player);