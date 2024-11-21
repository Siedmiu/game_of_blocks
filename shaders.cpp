#include "shaders.h"

shaders::shaders() {
    //kompilacja shaderow
    const std::vector<std::string> shaderFiles = {
        "vertex_shader.glsl",
        "fragment_shader.glsl",
        "geometry_shader.glsl",
        "framebuffer_basic_fragment_shader.glsl",
        "framebuffer_universal_vertex_shader.glsl",
        "fb_fs_blur.glsl",
        "fb_fs_B_and_W.glsl",
        "fb_fs_intensity_gradient.glsl",
        "fb_fs_magnitude_thresholding.glsl",
        "fb_fs_hysteresis.glsl"
    };

    auto shaderCodes = loadShaderFiles(shaderFiles);

    const char* vertexShaderCode = shaderCodes["vertex_shader.glsl"].c_str();
    const char* fragmentShaderCode = shaderCodes["fragment_shader.glsl"].c_str();
    const char* geometryShaderCode = shaderCodes["geometry_shader.glsl"].c_str();

    const char* framebufferBasicFragmentShaderCode = shaderCodes["framebuffer_basic_fragment_shader.glsl"].c_str();
    const char* framebufferUniversalVertexShaderCode = shaderCodes["framebuffer_universal_vertex_shader.glsl"].c_str();

    const char* blurFragmentShaderCode = shaderCodes["fb_fs_blur.glsl"].c_str();
    const char* BAndWFragmentShaderCode = shaderCodes["fb_fs_B_and_W.glsl"].c_str();
    const char* intensityGradientFragmentShaderCode = shaderCodes["fb_fs_intensity_gradient.glsl"].c_str();
    const char* magnitudeThreasholdingFragmentShaderCode = shaderCodes["fb_fs_magnitude_thresholding.glsl"].c_str();
    const char* edgeTrackingByHysteresisFragmentShaderCode = shaderCodes["fb_fs_hysteresis.glsl"].c_str();

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
    glShaderSource(geometryShader, 1, &geometryShaderCode, NULL);
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

    //UNIVERSAL VERTEX SHADER
    framebufferUniversalVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(framebufferUniversalVertexShader, 1, &framebufferUniversalVertexShaderCode, NULL);
    glCompileShader(framebufferUniversalVertexShader);
    checkCompileErrors(framebufferUniversalVertexShader, "FRAMEBUFFER_VERTEX");

    //BASIC FRAMEBUFFER SHADER
    framebufferBasicFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(framebufferBasicFragmentShader, 1, &framebufferBasicFragmentShaderCode, NULL);
    glCompileShader(framebufferBasicFragmentShader);
    checkCompileErrors(framebufferBasicFragmentShader, "FRAMEBUFFER_FRAGMENT");

    framebufferBasicShaderProgram = glCreateProgram();
    glAttachShader(framebufferBasicShaderProgram, framebufferUniversalVertexShader);
    glAttachShader(framebufferBasicShaderProgram, framebufferBasicFragmentShader);
    glLinkProgram(framebufferBasicShaderProgram);
    checkCompileErrors(framebufferBasicShaderProgram, "PROGRAM");

    glDeleteShader(framebufferBasicFragmentShader);

    //BLUR SHADER
    blurFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(blurFragmentShader, 1, &blurFragmentShaderCode, NULL);
    glCompileShader(blurFragmentShader);
    checkCompileErrors(blurFragmentShader, "BLUR_FRAGMENT");

    blurShaderProgram = glCreateProgram();
    glAttachShader(blurShaderProgram, framebufferUniversalVertexShader);
    glAttachShader(blurShaderProgram, blurFragmentShader);
    glLinkProgram(blurShaderProgram);
    checkCompileErrors(blurShaderProgram, "PROGRAM");

    glDeleteShader(blurFragmentShader);

    //B&W SHADER
    BAndWFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(BAndWFragmentShader, 1, &BAndWFragmentShaderCode, NULL);
    glCompileShader(BAndWFragmentShader);
    checkCompileErrors(BAndWFragmentShader, "BW_FRAGMENT");

    BAndWShaderProgram = glCreateProgram();
    glAttachShader(BAndWShaderProgram, framebufferUniversalVertexShader);
    glAttachShader(BAndWShaderProgram, BAndWFragmentShader);
    glLinkProgram(BAndWShaderProgram);
    checkCompileErrors(BAndWShaderProgram, "PROGRAM");

    glDeleteShader(BAndWFragmentShader);

    //INTENSITY GRADIENT SHADER
    intensityGradientFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(intensityGradientFragmentShader, 1, &intensityGradientFragmentShaderCode, NULL);
    glCompileShader(intensityGradientFragmentShader);
    checkCompileErrors(intensityGradientFragmentShader, "IGS_FRAGMENT");

    intensityGradientShaderProgram = glCreateProgram();
    glAttachShader(intensityGradientShaderProgram, framebufferUniversalVertexShader);
    glAttachShader(intensityGradientShaderProgram, intensityGradientFragmentShader);
    glLinkProgram(intensityGradientShaderProgram);
    checkCompileErrors(intensityGradientShaderProgram, "PROGRAM");

    glDeleteShader(intensityGradientFragmentShader);

    //MAGNITUDE THREASHOLDING SHADER
    magnitudeThreasholdingFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(magnitudeThreasholdingFragmentShader, 1, &magnitudeThreasholdingFragmentShaderCode, NULL);
    glCompileShader(magnitudeThreasholdingFragmentShader);
    checkCompileErrors(magnitudeThreasholdingFragmentShader, "MT_FRAGMENT");

    magnitudeThreasholdingShaderProgram = glCreateProgram();
    glAttachShader(magnitudeThreasholdingShaderProgram, framebufferUniversalVertexShader);
    glAttachShader(magnitudeThreasholdingShaderProgram, magnitudeThreasholdingFragmentShader);
    glLinkProgram(magnitudeThreasholdingShaderProgram);
    checkCompileErrors(magnitudeThreasholdingShaderProgram, "PROGRAM");

    glDeleteShader(magnitudeThreasholdingFragmentShader);

    //EDGE TRACKING BY HYSTERESIS
    edgeTrackingByHysteresisFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(edgeTrackingByHysteresisFragmentShader, 1, &edgeTrackingByHysteresisFragmentShaderCode, NULL);
    glCompileShader(edgeTrackingByHysteresisFragmentShader);
    checkCompileErrors(edgeTrackingByHysteresisFragmentShader, "MT_FRAGMENT");

    edgeTrackingByHysteresisShaderProgram = glCreateProgram();
    glAttachShader(edgeTrackingByHysteresisShaderProgram, framebufferUniversalVertexShader);
    glAttachShader(edgeTrackingByHysteresisShaderProgram, edgeTrackingByHysteresisFragmentShader);
    glLinkProgram(edgeTrackingByHysteresisShaderProgram);
    checkCompileErrors(edgeTrackingByHysteresisShaderProgram, "PROGRAM");

    glDeleteShader(edgeTrackingByHysteresisFragmentShader);

    //delete universal vertex shader
    glDeleteShader(framebufferUniversalVertexShader);
}

std::unordered_map<std::string, std::string> shaders::loadShaderFiles(const std::vector<std::string>& shaderFiles) {
    std::unordered_map<std::string, std::string> shaderCodes;

    for (const auto& filename : shaderFiles) {
        std::ifstream shaderFile;
        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            //shaderFile.open("../shaders/" + filename);
            shaderFile.open(filename);

            std::stringstream shaderStream;
            shaderStream << shaderFile.rdbuf();

            shaderFile.close();
            shaderCodes[filename] = shaderStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: "
                << filename << " - " << e.what() << std::endl;
        }
    }
    return shaderCodes;
}

unsigned int shaders::shaderProgramID() const {
    return shaderProgram;
}

unsigned int shaders::framebufferShaderProgramID() const {
    return framebufferBasicShaderProgram;
}

unsigned int shaders::blurShaderProgramID() const {
    return blurShaderProgram;
}

unsigned int shaders::blackAndWhiteShaderProgramID() const {
    return BAndWShaderProgram;
}

unsigned int shaders::intensityGradientShaderProgramID() const {
    return intensityGradientShaderProgram;
}

unsigned int shaders::magnitudeThreasholdingShaderProgramID() const {
    return magnitudeThreasholdingShaderProgram;
}

unsigned int shaders::edgeTrackingByHysteresisShaderProgramID() const {
    return edgeTrackingByHysteresisShaderProgram;
}

void shaders::use(unsigned int shaderProgramID) const {
    glUseProgram(shaderProgramID);
}
void shaders::setInt(unsigned int shaderProgramID, const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), value);
}
void shaders::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void shaders::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
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