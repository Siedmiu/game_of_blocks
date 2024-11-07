#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <iostream>

class shaders {
private:
	//shadery (kompilowane runtime)
	unsigned int vertexShader, fragmentShader, shaderProgram;

public:
	shaders();

	void use() const;
	unsigned int shaderProgramID() const;
	void checkCompileErrors(GLuint shader, std::string type);
	void setMat4(const std::string& name, const glm::mat4& mat) const;
	void setInt(const std::string& name, int value) const;
};