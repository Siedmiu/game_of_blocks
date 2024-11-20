#version 430 core

out vec4 fColour;
in vec2 fTexCoords;

uniform sampler2D inputTexture;
uniform vec2 screenSize;

const float PIXEL_WIDTH_OFFSET = 1.0f / screenSize.x;
const float PIXEL_HEIGHT_OFFSET = 1.0f / screenSize.y;

const vec2 PIXEL_OFFSETS_3x3[9] = vec2[] (
	vec2(-PIXEL_WIDTH_OFFSET, PIXEL_HEIGHT_OFFSET),  vec2( 0, PIXEL_HEIGHT_OFFSET),  vec2(PIXEL_WIDTH_OFFSET, PIXEL_HEIGHT_OFFSET),
	vec2(-PIXEL_WIDTH_OFFSET, 0),                    vec2(-0, 0),                    vec2(PIXEL_WIDTH_OFFSET, 0),
	vec2(-PIXEL_WIDTH_OFFSET, -PIXEL_HEIGHT_OFFSET), vec2( 0, -PIXEL_HEIGHT_OFFSET), vec2(PIXEL_WIDTH_OFFSET, -PIXEL_HEIGHT_OFFSET)
);

const float BLUR_KERNEL_3x3[9] = float[] (
	1.0f, 2.0f, 1.0f,
	2.0f, 4.0f, 2.0f,
	1.0f, 2.0f, 1.0f
);
const float kernelSum = 16.0f;

void main() {
	vec3 colourRGB = vec3(0.0f);

	for(int i = 0; i < 9; i++) {
		colourRGB += vec3(texture(inputTexture, fTexCoords.st + PIXEL_OFFSETS_3x3[i])) * BLUR_KERNEL_3x3[i];
	}

	fColour = vec4(colourRGB / kernelSum, 1.0f);
	//fColour = texture(inputTexture, fTexCoords);
}