#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

class shaders {
private:
	//shadery (kompilowane runtime)
	unsigned int vertexShader, fragmentShader, geometryShader, shaderProgram;
	unsigned int framebufferUniversalVertexShader, framebufferBasicFragmentShader, framebufferBasicShaderProgram;

	unsigned int blurFragmentShader, blurShaderProgram;
	unsigned int BAndWFragmentShader, BAndWShaderProgram;
	unsigned int intensityGradientFragmentShader, intensityGradientShaderProgram;
	unsigned int magnitudeThreasholdingFragmentShader, magnitudeThreasholdingShaderProgram;
	unsigned int edgeTrackingByHysteresisFragmentShader, edgeTrackingByHysteresisShaderProgram;

	unsigned int basicKuwaharaFragmentShader, basicKuwaharaShaderProgram;

	unsigned int cannyOverlayFragmentShader, cannyOverlayShaderProgram;

	void checkCompileErrors(GLuint shader, std::string type);
	std::unordered_map<std::string, std::string> loadShaderFiles(const std::vector<std::string>& shaderFiles);

public:
	shaders();

	void use(unsigned int shaderProgramID) const;
	unsigned int shaderProgramID() const;
	unsigned int framebufferShaderProgramID() const;

	unsigned int blurShaderProgramID() const;
	unsigned int blackAndWhiteShaderProgramID() const;
	unsigned int intensityGradientShaderProgramID() const;
	unsigned int magnitudeThreasholdingShaderProgramID() const;
	unsigned int edgeTrackingByHysteresisShaderProgramID() const;
	unsigned int basicKuwaharaShaderProgramID() const;
	unsigned int cannyOverlayShaderProgramID() const;

	void setMat4(const std::string& name, const glm::mat4& mat) const;
	void setInt(unsigned int shaderProgramID, const std::string& name, int value) const;
};