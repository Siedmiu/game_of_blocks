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

const float SOBEL_OPERATOR_X[9] = float[] (
	1.0f, 0.0f, -1.0f,
	2.0f, 0.0f, -2.0f,
	1.0f, 0.0f, -1.0f
);

const float SOBEL_OPERATOR_Y[9] = float[] (
	 1.0f,  2.0f,  1.0f,
	 0.0f,  0.0f,  0.0f,
	-1.0f, -2.0f, -1.0f
);

void main() {
	float sobelX = 0.0f, sobelY = 0.0f;

	//pixel convolution (splot) with sobel operator
	for(int i = 0; i < 9; i++) {
		float pixel = texture(inputTexture, fTexCoords.st + PIXEL_OFFSETS_3x3[i]).r;
		//image is black and white
		sobelX += pixel.x * SOBEL_OPERATOR_X[i];
		sobelY += pixel.x * SOBEL_OPERATOR_Y[i];
	}

	float intensity = sqrt(sobelX * sobelX + sobelY * sobelY);
	fColour = vec4(vec3(intensity), 1.0f);
}