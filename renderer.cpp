#include "renderer.h"
#include "shaders.h"

renderer::renderer(unsigned int shaderProgramID, const world& gameWorld)
    : gameWorld(gameWorld) {
    timeValue = 0.0;
    shaderProgram = shaderProgramID;
    model = glm::mat4(1.0f);
}

void renderer::render(unsigned int VAO) {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    const auto& chunks = gameWorld.getChunks();
    for (const auto& chunkPair : chunks) {
        const auto& chunk = chunkPair.second;
        if (chunk->needsUpdate) continue;

        glm::vec3 chunkOffset(chunk->chunkX * CHUNK_LENGTH, 0, chunk->chunkY * CHUNK_LENGTH);

        model = glm::mat4(1.0f);
        model = glm::translate(model, chunkOffset);
        setMat4("model", model);

        glBindVertexArray(chunk->VAO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, chunk->SSBO);
        glDrawArrays(GL_POINTS, 0, chunk->vertexCount);
        glBindVertexArray(0);
    }
}

void renderer::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void renderer::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}