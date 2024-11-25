#version 330 core

out vec4 fColour;
in vec2 fTexCoords;

uniform sampler2D inputTexture;
uniform sampler2D cannyTexture;

void main() {
    const float SPLIT_SCREEN = 0.5;

    //branching logic but i dont care
    if (fTexCoords.x <= SPLIT_SCREEN) {
        fColour = texture(inputTexture, fTexCoords);
    } else {
        fColour = texture(cannyTexture, fTexCoords);
    }
}