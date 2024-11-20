#version 430 core

out vec4 fColour;
in vec2 fTexCoords;

uniform sampler2D screenTexture0;
uniform vec2 screenSize;

const float PIXEL_WIDTH_OFFSET = 1.0f / screenSize.x;
const float PIXEL_HEIGHT_OFFSET = 1.0f / screenSize.y;

const vec2 PIXEL_OFFSETS_3x3[9] = vec2[] (
	vec2(-PIXEL_WIDTH_OFFSET,  PIXEL_HEIGHT_OFFSET), vec2( 0,  PIXEL_HEIGHT_OFFSET), vec2(PIXEL_WIDTH_OFFSET,  PIXEL_HEIGHT_OFFSET),
	vec2(-PIXEL_WIDTH_OFFSET,  0),                   vec2(-0,  0),                   vec2(PIXEL_WIDTH_OFFSET,  0),
	vec2(-PIXEL_WIDTH_OFFSET, -PIXEL_HEIGHT_OFFSET), vec2( 0, -PIXEL_HEIGHT_OFFSET), vec2(PIXEL_WIDTH_OFFSET, -PIXEL_HEIGHT_OFFSET)
);

void main() {
	vec4 pixel = texture(screenTexture0, fTexCoords);
	float intensity = 0.333 * (pixel.r + pixel.g + pixel.b);
	fColour = vec4(vec3(intensity), 1);
}