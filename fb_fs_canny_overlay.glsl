#version 330 core
out vec4 fColour;
in vec2 fTexCoords;

uniform sampler2D inputTexture;
uniform sampler2D cannyTexture;

//black outline
const bool INVERSE_CANNY = false;

void main() {
    vec4 originalColor = texture(inputTexture, fTexCoords.st);
    vec4 cannyTexture = texture(cannyTexture, fTexCoords.st);

    if (INVERSE_CANNY) {
        vec4 blackOutline = vec4(0.0, 0.0, 0.0, 1.0);
        fColour = mix(originalColor, blackOutline, cannyTexture.r * 2);
    } else {
        fColour = originalColor * step(cannyTexture.x, 0) + cannyTexture * 2;
    }
}