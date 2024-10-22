//https://iquilezles.org/articles/morenoise/
//https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/procedural-patterns-noise-part-1/creating-simple-2D-noise.html
//https://learnopengl.com/Lighting/Basic-Lighting

//do zrobienia
// naprawic tekstury
// kolizja i chodzenie
// audio
// 
// moze:
// naprawic oktawy
// wielowatkowy generator
//

//block id
//255 = air
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

#include <iostream>
//#include <thread>

const float SCR_WIDTH = 1920; //1280
const float SCR_HEIGHT = 1080; //720
const float FOV = 70.0f;
const float FPS = 60.0f; //nie wplywa na szybkosc symulacji
const float TIMESTEP = 1.0f / FPS;

const unsigned int NUMBER_OF_TEXTURES = 3;

/*bool running = true;
void chunkGenerationThread(world& world) {
    while (running) {
        world.createChunks();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}*/

int main() {
    //glfw inicjalizacja
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //kompilacja shaderow
    shaders shaders;

    unsigned int shaderProgram;
    shaderProgram = shaders.shaderProgramID();

    //tekstury     //co to tu robi trzeba to wywalic do innego pliku bo balagan jest
    unsigned int textures[NUMBER_OF_TEXTURES];
    glGenTextures(NUMBER_OF_TEXTURES, textures);

    //0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("grass_block_top.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture 0" << std::endl;
    }
    stbi_image_free(data);

    //1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    data = stbi_load("dirt.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture 1" << std::endl;
    }
    stbi_image_free(data);

    //2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    data = stbi_load("stone.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture 2" << std::endl;
    }
    stbi_image_free(data);

    shaders.use();
    shaders.setInt("texturesArray[0]", 0);
    shaders.setInt("texturesArray[1]", 1);
    shaders.setInt("texturesArray[2]", 2);

    //std::cout << glGetUniformLocation(shaderProgram, "texturesArray[2]") << std::endl;

    //generacja ID vbo (obiektu buferowego wierzcholkow) i przypisanie typu
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //-----------------MACIERZE TRANSFORMACJI----------------------------//
    //macierz modelu
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    //macierz widoku
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    //projekcja prespektywa
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    //projection = glm::ortho(0.0f, 100.0f, 0.0f, 100.0f, 0.1f, 1000.0f);

    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

    shaders.setMat4("projection", projection);
    //-------------------------------------------------------------------//

    //player
    glm::vec3 playerXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
    player player;

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

    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),         // [ rx ry rz 0 ]
        glm::vec3(0.0f, 0.0f, 0.0f),         // [ ux uy uz 0 ]
        glm::vec3(0.0f, 1.0f, 0.0f));        // [ dx dy dz 0 ]
    // [ 0  0  0  1 ]
//initial positon
    playerXYZ = cam.getXYZ();
    player.updatePlayerPosition(playerXYZ.x, playerXYZ.y, playerXYZ.z);

    //G R A W I T A C J A
    const float GRAVITY = 9.81f * 3, JUMP_SPEED = -10.0f;
    float fallingVelocity = 0.0f;

    //game loop
    while (!glfwWindowShouldClose(window)) {
        //time
        newTime = (float)glfwGetTime();
        frameTime = newTime - currentTime;
        currentTime = newTime;
        timeAccumulator += frameTime;

        //generate chunks around player
        world.createChunks();

        //clear zbuffer
        glClearColor(0.2f, 0.4f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //transformacje i time step
        while (timeAccumulator >= TIMESTEP) {
            //movement
            processInput(window, TIMESTEP, cam);

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
            }

            //AABB Colision detection only if player moves
            playerXYZ = cam.getXYZ();
            player.updatePlayerPosition(playerXYZ.x, playerXYZ.y, playerXYZ.z);
            world.AABBcolisionDetection();

            glm::mat4 view = cam.getViewMatrix();
            shaders.setMat4("view", view);

            timeAccumulator -= TIMESTEP;
        }

        //render
        renderer.render(VAO);

        // (podwojny buffer), sprawdzanie eventow
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //running = false;
    //chunkThread.join();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
}