#include "shaders.h"

shaders::shaders() {
    //kompilacja shaderow

    std::string vertexCode;
    std::string fragmentCode;
    std::string geometrCode;
    std::string framebufferFragmentCode;
    std::string framebufferVertexCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    std::ifstream framebufferFragmentShaderFile;
    std::ifstream framebufferVertexShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    framebufferFragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    framebufferVertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vShaderFile.open("vertex_shader.glsl");
        fShaderFile.open("fragment_shader.glsl");
        gShaderFile.open("geometry_shader.glsl");
        framebufferFragmentShaderFile.open("framebuffer_fragment_shader.glsl");
        framebufferVertexShaderFile.open("framebuffer_vertex_shader.glsl");

        std::stringstream vShaderStream, fShaderStream, gShaderStream, framebufferFragmentShaderStream, framebufferVertexShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();
        framebufferFragmentShaderStream << framebufferFragmentShaderFile.rdbuf();
        framebufferVertexShaderStream << framebufferVertexShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();
        gShaderFile.close();
        framebufferFragmentShaderFile.close();
        framebufferVertexShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        geometrCode = gShaderStream.str();
        framebufferFragmentCode = framebufferFragmentShaderStream.str();
        framebufferVertexCode = framebufferVertexShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    const char* vertexShaderCode = vertexCode.c_str();
    const char* fragmentShaderCode = fragmentCode.c_str();
    const char* geometrShaderCode = geometrCode.c_str();
    const char* framebufferFragmentShaderCode = framebufferFragmentCode.c_str();
    const char* framebufferVertexShaderCode = framebufferVertexCode.c_str();

    //BASE SHADER
    //id shadera
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //powiazanie kodu shadera
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");
    
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");
    
    geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &geometrShaderCode, NULL);
    glCompileShader(geometryShader);
    checkCompileErrors(geometryShader, "GEOMETR");

    //program shader (polaczenie)
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, geometryShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);

    //FRAMEBUFFER SHADER
    framebufferVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(framebufferVertexShader, 1, &framebufferVertexShaderCode, NULL);
    glCompileShader(framebufferVertexShader);
    checkCompileErrors(framebufferVertexShader, "FRAMEBUFFER_VERTEX");

    framebufferFragmentShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(framebufferFragmentShader, 1, &framebufferFragmentShaderCode, NULL);
    glCompileShader(framebufferFragmentShader);
    checkCompileErrors(framebufferFragmentShader, "FRAMEBUFFER_FRAGMENT");

    fragmentShaderProgram = glCreateProgram();
    glAttachShader(fragmentShaderProgram, framebufferVertexShader);
    glAttachShader(fragmentShaderProgram, framebufferFragmentShader);
    glLinkProgram(fragmentShaderProgram);
    checkCompileErrors(fragmentShaderProgram, "FRAMEBUFFER_PROGRAM");

    glDeleteShader(framebufferVertexShader);
    glDeleteShader(framebufferFragmentShader);
}

unsigned int shaders::shaderProgramID() const {
    return shaderProgram;
}

unsigned int shaders::framebufferShaderProgramID() const {
    return fragmentShaderProgram;
}

void shaders::use() const {
    glUseProgram(shaderProgram);
}
void shaders::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}
void shaders::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void shaders::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM" && type != "FRAMEBUFFER_PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}