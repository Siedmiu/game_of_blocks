//derivative terrain generation https://iquilezles.org/articles/morenoise/
//noise generation https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/procedural-patterns-noise-part-1/introduction.html
//learnOpenGL my beloved https://learnopengl.com/Lighting/Basic-Lighting
//colision detection therory https://gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/
//post processing https://www.youtube.com/watch?v=RepvBIfpcwE
//image processing convolution https://www.youtube.com/watch?v=KuXjwB4LzSA
//canny edge detection https://en.wikipedia.org/wiki/Canny_edge_detector

//do zrobienia
// kolizja i chodzenie
// z pre pass
// Shaders
// 
// moze:
// naprawic oktawy
//
//block id:
//32 = air
//0 = grass
//1 = dirt
//2 = stone

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders.h"
#include "window.h"
#include "renderer.h"
#include "camera.h"
#include "stb_image.h"
#include "world.h"
#include "player.h"
#include "texture_loader.h"
#include "errorReporting.h"

#include <iostream>

const int SCR_WIDTH = 1920; //1280
const int SCR_HEIGHT = 1080; //720
const float FOV = 70.0f;
const float TIMESTEP = 1.0f / 60.0f;

int main() {
    //glfw inicjalizacja
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    //tworzenie glfw window instance
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "kloce", NULL, NULL);
    if (window == NULL) {
        std::cout << "Stworzenie okna GLFW nieudane" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //camera
    camera& cam = camera::getCamera();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);


    //inicjalizacja glad PRZED urzyciem OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Nieudana inicjalizacja GLAD" << std::endl;
        return -1;
    }

    //OPENGL------------------------------------------------------------------------
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    enableReportGlErrors();

    //kompilacja shaderow
    shaders shaders;

    unsigned int shaderProgram, framebufferShaderProgram;
    shaderProgram = shaders.shaderProgramID();
    framebufferShaderProgram = shaders.framebufferShaderProgramID();

    //tekstury
    TextureLoader textureLoader;
    std::vector<std::string> textureFiles = {
        "grass_block_top.png",
        "dirt.png",
        "stone.png",
        "missing.png"
    };

    std::vector<unsigned int> textures = textureLoader.loadTextures(textureFiles);

    shaders.use(shaderProgram);
    for (int i = 0; i < textures.size(); i++) {
        shaders.setInt(shaderProgram, "texturesArray[" + std::to_string(i) + "]", i + 1);
    }

    //vertices storage objects
    unsigned int SSBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &SSBO);

    //put this into a new file
    //frame buffer and its texture for post processing effects
    shaders.use(framebufferShaderProgram);

    unsigned int FBO, framebufferTexture;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &framebufferTexture);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

    //render buffer
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    //CCW winding order
    float framebufferVertices[24] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int framebufferVAO, framebufferVBO;
    glGenVertexArrays(1, &framebufferVAO);
    glGenBuffers(1, &framebufferVBO);
    glBindVertexArray(framebufferVAO);
    glBindBuffer(GL_ARRAY_BUFFER, framebufferVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(framebufferVertices), &framebufferVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    shaders.setInt(framebufferShaderProgram, "screenTexture", 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FRAMEBUFFER NOT COMPLETE" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //end

    //Edging mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //-----------------TRANSFORMATION MATRICES----------------------------//
    shaders.use(shaderProgram);
    //macierz modelu
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    //macierz widoku
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    //projekcja prespektywa
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    //projection = glm::ortho(0.0f, 10.0f, 0.0f, 10.0f, 0.1f, 1000.0f);

    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

    shaders.setMat4("projection", projection);
    //-------------------------------------------------------------------//

    //player
    player player(cam);

    //inicjalizacja swiata
    world world(player, cam);
    //std::thread chunkThread(chunkGenerationThread, std::ref(world));

    //inicjalizacja renderer
    renderer renderer(shaderProgram, world);

    //time
    float timeAccumulator = 0.0f;
    float currentTime = (float)glfwGetTime();
    float newTime = (float)glfwGetTime();
    float frameTime = newTime - currentTime;

    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), // [ rx ry rz 0 ]
                       glm::vec3(0.0f, 0.0f, 0.0f), // [ ux uy uz 0 ]
                       glm::vec3(0.0f, 1.0f, 0.0f));// [ dx dy dz 0 ]
                                                    // [ 0  0  0  1 ]

    //initial positon
    player.updatePlayerPositionVector(cam.getXYZ());
    world.updatePlayerPosition();

    //gravity move to velocity
    //const float GRAVITATIONAL_CONSTANT = 9.81f;

    //game loop
    while (!glfwWindowShouldClose(window)) {
        //time
        newTime = (float)glfwGetTime();
        frameTime = newTime - currentTime;
        currentTime = newTime;
        timeAccumulator += frameTime;

        //generate chunks around player
        world.createChunks();

        //bind framebuffer before drawing anything
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        //clear zbuffer
        glClearColor(0.2f, 0.4f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        //transformacje i time step
        while (timeAccumulator >= TIMESTEP) {
            //movement input
            processInput(window, TIMESTEP, player);

            /*
            //Gravity
            if (!world.onTheFloor) {
                fallingVelocity += GRAVITY * TIMESTEP;
                //cam.moveDown(fallingVelocity * TIMESTEP);
            }
            else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                fallingVelocity = JUMP_SPEED;
            }
            else {
                fallingVelocity = 0.0f;
            }*/

            //Colision detection
            //world.sweptAABBcolisonCheck();
            
            //handle velocity, air resistance, friction and gravity
            player.movePlayer(TIMESTEP);
            world.updatePlayerPosition();

            glUseProgram(shaderProgram);
            glm::mat4 view = cam.getViewMatrix();
            shaders.setMat4("view", view);

            timeAccumulator -= TIMESTEP;
        }

        //render
        renderer.render(VAO);

        //make this a postprocess function
        //framebuffer post process
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(framebufferShaderProgram);
        glBindVertexArray(framebufferVAO);
        glDisable(GL_DEPTH_TEST);

        glBindTexture(GL_TEXTURE_2D, framebufferTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //

        // (podwojny buffer), sprawdzanie eventow
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &framebufferVAO);
    glDeleteBuffers(1, &SSBO);
    glDeleteBuffers(1, &FBO);
    glDeleteBuffers(1, &framebufferVBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
}