#version 430 core

out vec4 fColour;
in vec2 fTexCoords;

uniform sampler2D screenTexture0;

const bool BLACK_AND_WHITE = true;
const bool INVERSE_COLOURS = false;
const bool EDGE_DETECTION = false;

const float PIXEL_WIDTH_OFFSET = 1.0f / 1920.0f;
const float PIXEL_HEIGHT_OFFSET = 1.0f / 1080.0f;

const vec2 PIXEL_OFFSETS_3x3[9] = vec2[] (
	vec2(-PIXEL_WIDTH_OFFSET,  PIXEL_HEIGHT_OFFSET), vec2( 0,  PIXEL_HEIGHT_OFFSET), vec2(PIXEL_WIDTH_OFFSET,  PIXEL_HEIGHT_OFFSET),
	vec2(-PIXEL_WIDTH_OFFSET,  0),                   vec2(-0,  0),                   vec2(PIXEL_WIDTH_OFFSET,  0),
	vec2(-PIXEL_WIDTH_OFFSET, -PIXEL_HEIGHT_OFFSET), vec2( 0, -PIXEL_HEIGHT_OFFSET), vec2(PIXEL_WIDTH_OFFSET, -PIXEL_HEIGHT_OFFSET)
);

const float EDGE_DETECTION_KERNEL_3x3[9] = float[] (
	1.0f,  1.0f, 1.0f,
	1.0f, -8.0f, 1.0f,
	1.0f,  1.0f, 1.0f
);

void main() {
	if (BLACK_AND_WHITE) {
		vec4 pixel = texture(screenTexture0, fTexCoords);
		float intensity = 0.333 * (pixel.r + pixel.g + pixel.b);
		fColour = vec4(vec3(intensity), 1);

	} else if (INVERSE_COLOURS) {
		fColour = vec4(1.0f, 1.0f, 1.0f, 2.0f ) - texture(screenTexture0, fTexCoords);

	} else if (EDGE_DETECTION) {
		vec3 colourRGB = vec3(0.0f);
		for(int i = 0; i < 9; i++) {
			colourRGB += vec3(texture(screenTexture0, fTexCoords.st + PIXEL_OFFSETS_3x3[i])) * EDGE_DETECTION_KERNEL_3x3[i];
		}
		fColour = vec4(colourRGB, 1.0f);

	} else {
		fColour = texture(screenTexture0, fTexCoords);
	}
}