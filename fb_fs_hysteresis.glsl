#version 430 core

out vec4 fColour;
in vec2 fTexCoords;

uniform sampler2D inputTexture;
uniform vec2 screenSize;

const float PIXEL_WIDTH_OFFSET = 1.0f / screenSize.x;
const float PIXEL_HEIGHT_OFFSET = 1.0f / screenSize.y;

const vec2 PIXEL_OFFSETS_3x3[9] = vec2[](
	vec2(-PIXEL_WIDTH_OFFSET, PIXEL_HEIGHT_OFFSET),  vec2(0, PIXEL_HEIGHT_OFFSET),  vec2(PIXEL_WIDTH_OFFSET, PIXEL_HEIGHT_OFFSET),
	vec2(-PIXEL_WIDTH_OFFSET, 0),                    vec2(-0, 0),                   vec2(PIXEL_WIDTH_OFFSET, 0),
	vec2(-PIXEL_WIDTH_OFFSET, -PIXEL_HEIGHT_OFFSET), vec2(0, -PIXEL_HEIGHT_OFFSET), vec2(PIXEL_WIDTH_OFFSET, -PIXEL_HEIGHT_OFFSET)
);

const float HYSTERESIS_HIGH = 0.3f;

void main() {
	//hysteresis
	float middlePixel = texture(inputTexture, fTexCoords).r;
	float neighbouringPixel = 0.0f;
	float maxPixel = 0.0f;

	for (int i = 0; i < 9; i++) {
		neighbouringPixel = texture(inputTexture, fTexCoords.st + PIXEL_OFFSETS_3x3[i]).r;
		maxPixel = max(maxPixel, neighbouringPixel);
	}

	middlePixel = mix(0.0f, middlePixel, step(HYSTERESIS_HIGH, maxPixel));

//output colour
	fColour = vec4(vec3(middlePixel), 1.0f);
}