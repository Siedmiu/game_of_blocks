#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <iostream>

class shaders {
private:
	//shadery (kompilowane runtime)
	const char* vertexShaderSource = 
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec2 aTexCoord;\n"
		"layout(location = 2) in float aTexID;\n"

		"out vec2 TexCoord;\n"
		"out float TexID;\n"

		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"

		"void main()\n"
		"{\n"
		"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
		"	TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
		"	TexID = aTexID;\n"
		"}\0";

	const char* fragmentShaderSource =
		"#version 330 core\n"

		"out vec4 FragColor;\n"

		"in vec2 TexCoord;\n"
		"in float TexID;\n"

		"uniform sampler2D texturesArray[3];\n"

		"void main()\n"
		"{\n"
		"    if (TexID == 0.0) {\n"
		"        FragColor = texture(texturesArray[0], TexCoord);\n"
		"    } else if (TexID == 1.0) {\n"
		"        FragColor = texture(texturesArray[1], TexCoord);\n"
		"    } else if (TexID == 2.0) {\n"
		"        FragColor = texture(texturesArray[1], TexCoord);\n"
		"    } else {\n"
		"        FragColor = texture(texturesArray[1], TexCoord);\n" //na wypadek bledu konwersji TexID
		"    }\n"
		"}\n\0";

	unsigned int vertexShader, fragmentShader, shaderProgram;

public:
	shaders();

	void use() const;
	unsigned int shaderProgramID() const;
	void checkCompileErrors(GLuint shader, std::string type);
	void setMat4(const std::string& name, const glm::mat4& mat) const;
	void setInt(const std::string& name, int value) const;
};