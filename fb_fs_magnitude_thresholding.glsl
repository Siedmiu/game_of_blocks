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

const float TOLERANCE = 0.05f;

const bool DOBUBLE_THREASHOLDING = true;
const float LOW_THRESHOLD = 0.1f, HIGH_THRESHOLD = 0.25f;
const float THRESHOLD_CONTRAST_HIGH = 0.3f, THRESHOLD_CONTRAST_LOW = 0.1f;

void main() {
//magnitude threasholding
	float middlePixel = texture(inputTexture, fTexCoords).r;
	float neighbouringPixel = 0.0f;
	float maxPixel = 0.0f;

	for (int i = 0; i < 9; i++) {
		neighbouringPixel = texture(inputTexture, fTexCoords.st + PIXEL_OFFSETS_3x3[i]).r;
		maxPixel = max(maxPixel, neighbouringPixel);
	}

	//if statement "if (maxPixel > middlePixel) middlePixel = 0.0f"
	middlePixel *= step(maxPixel - TOLERANCE, middlePixel);
//

//double threasholding
if (DOBUBLE_THREASHOLDING) {
	//if (pixel >= HIGH_THRESHOLD) pixel = HIGH_THRESHOLD
	middlePixel = mix(middlePixel, HIGH_THRESHOLD + THRESHOLD_CONTRAST_HIGH, step(HIGH_THRESHOLD, middlePixel));

	//if (HIGH_THRESHOLD > pixel >= LOW_THRESHOLD) pixel = LOW_THRESHOLD
	middlePixel = mix(middlePixel, LOW_THRESHOLD + THRESHOLD_CONTRAST_LOW, step(LOW_THRESHOLD, middlePixel) * (1.0 - step(HIGH_THRESHOLD, middlePixel)));

	//if (pixel <= LOW_THRESHOLD) pixel = 0
	middlePixel = mix( 0.0f, middlePixel, step(LOW_THRESHOLD, middlePixel));
}

//output colour
	fColour = vec4(vec3(middlePixel), 1.0f);
}