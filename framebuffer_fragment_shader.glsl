#version 430 core

out vec4 fColour;
in vec2 fTexCoords;

uniform sampler2D screenTexture;

void main() {
	fColour = texture(screenTexture, fTexCoords);
}