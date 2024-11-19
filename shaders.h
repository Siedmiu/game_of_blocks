#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class shaders {
private:
	//shadery (kompilowane runtime)
	unsigned int vertexShader, fragmentShader, geometryShader, shaderProgram;
	unsigned int framebufferVertexShader, framebufferFragmentShader, framebufferShaderProgram;

	void checkCompileErrors(GLuint shader, std::string type);

public:
	shaders();

	void use(unsigned int shaderProgramID) const;
	unsigned int shaderProgramID() const;
	unsigned int framebufferShaderProgramID() const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
	void setInt(unsigned int shaderProgramID, const std::string& name, int value) const;
};