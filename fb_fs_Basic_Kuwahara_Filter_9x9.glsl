#version 430 core

out vec4 fColour;
in vec2 fTexCoords;

uniform sampler2D inputTexture;
uniform vec2 screenSize;

const float PIXEL_WIDTH_OFFSET = 1.0f / screenSize.x;
const float PIXEL_HEIGHT_OFFSET = 1.0f / screenSize.y;

//pixel offsets for a 9x9 kernel
const vec2 PIXEL_OFFSETS_9x9[81] = vec2[](                                                                                                                                                                                              //middle                       //
    vec2(-4 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET), vec2(0,  4 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET),
    vec2(-4 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET), vec2(0,  3 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET),
    vec2(-4 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET), vec2(0,  2 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET),
    vec2(-4 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET), vec2(0,  1 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET),
    vec2(-4 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(-3 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(-2 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(-1 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(0,  0),                       vec2(1 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(2 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(3 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(4 * PIXEL_WIDTH_OFFSET,  0),
    vec2(-4 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET), vec2(0, -1 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET),
    vec2(-4 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET), vec2(0, -2 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET),
    vec2(-4 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET), vec2(0, -3 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET),
    vec2(-4 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET), vec2(0, -4 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET)
);

//sectors for a 9x9 kernel divided into 5x5
const vec2 SECTOR_1[25] = vec2[](
    vec2(-4 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET), vec2(0,  4 * PIXEL_HEIGHT_OFFSET),
    vec2(-4 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET), vec2(0,  3 * PIXEL_HEIGHT_OFFSET),
    vec2(-4 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET), vec2(0,  2 * PIXEL_HEIGHT_OFFSET),
    vec2(-4 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET), vec2(0,  1 * PIXEL_HEIGHT_OFFSET),
    vec2(-4 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(-3 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(-2 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(-1 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(0,  0)
);

const vec2 SECTOR_2[25] = vec2[](
    vec2(-4 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(-3 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(-2 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(-1 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(0,  0),
    vec2(-4 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET), vec2(0, -1 * PIXEL_HEIGHT_OFFSET),
    vec2(-4 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET), vec2(0, -2 * PIXEL_HEIGHT_OFFSET),
    vec2(-4 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET), vec2(0, -3 * PIXEL_HEIGHT_OFFSET),
    vec2(-4 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET), vec2(-3 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET), vec2(-2 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET), vec2(-1 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET), vec2(0, -4 * PIXEL_HEIGHT_OFFSET)
);

const vec2 SECTOR_3[25] = vec2[](
    vec2(0,  4 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET,  4 * PIXEL_HEIGHT_OFFSET),
    vec2(0,  3 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET,  3 * PIXEL_HEIGHT_OFFSET),
    vec2(0,  2 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET,  2 * PIXEL_HEIGHT_OFFSET),
    vec2(0,  1 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET,  1 * PIXEL_HEIGHT_OFFSET),
    vec2(0,  0),                       vec2(1 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(2 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(3 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(4 * PIXEL_WIDTH_OFFSET,  0)
);

const vec2 SECTOR_4[25] = vec2[](
    vec2(0,  0),                       vec2(1 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(2 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(3 * PIXEL_WIDTH_OFFSET,  0)                      , vec2(4 * PIXEL_WIDTH_OFFSET,  0),
    vec2(0, -1 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET, -1 * PIXEL_HEIGHT_OFFSET),
    vec2(0, -2 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET, -2 * PIXEL_HEIGHT_OFFSET),
    vec2(0, -3 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET, -3 * PIXEL_HEIGHT_OFFSET),
    vec2(0, -4 * PIXEL_HEIGHT_OFFSET), vec2(1 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET), vec2(2 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET), vec2(3 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET), vec2(4 * PIXEL_WIDTH_OFFSET, -4 * PIXEL_HEIGHT_OFFSET)
);


void main() {
    //variance = (st. dev.)^2
    vec3 pixelVariance = vec3(0.0f);

    //sector 1
    vec3 sector1Sum = vec3(0.0f);
    for (int i = 0; i < 25; i++) {
		sector1Sum += texture(inputTexture, fTexCoords.st + SECTOR_1[i]).rgb;
	}
    vec3 sector1Average = sector1Sum * 0.04f;

    vec3 sector1VarianceSum = vec3(0.0f);
    for (int i = 0; i < 25; i++) {
		pixelVariance = (texture(inputTexture, fTexCoords.st + SECTOR_1[i]).rgb - sector1Average);
        pixelVariance *= pixelVariance;
        sector1VarianceSum += pixelVariance;
	}

    //sector 2
    vec3 sector2Sum = vec3(0.0f);
    for (int i = 0; i < 25; i++) {
		sector2Sum += texture(inputTexture, fTexCoords.st + SECTOR_2[i]).rgb;
	}
    vec3 sector2Average = sector2Sum * 0.04f;

    vec3 sector2VarianceSum = vec3(0.0f);
    for (int i = 0; i < 25; i++) {
		pixelVariance = (texture(inputTexture, fTexCoords.st + SECTOR_2[i]).rgb - sector2Average);
        pixelVariance *= pixelVariance;
        sector2VarianceSum += pixelVariance;
	}

    //sector 3
    vec3 sector3Sum = vec3(0.0f);
    for (int i = 0; i < 25; i++) {
		sector3Sum += texture(inputTexture, fTexCoords.st + SECTOR_3[i]).rgb;
	}
    vec3 sector3Average = sector3Sum * 0.04f;

    vec3 sector3VarianceSum = vec3(0.0f);
    for (int i = 0; i < 25; i++) {
		pixelVariance = (texture(inputTexture, fTexCoords.st + SECTOR_3[i]).rgb - sector3Average);
        pixelVariance *= pixelVariance;
        sector3VarianceSum += pixelVariance;
	}

    //sector 4
    vec3 sector4Sum = vec3(0.0f);
    for (int i = 0; i < 25; i++) {
		sector4Sum += texture(inputTexture, fTexCoords.st + SECTOR_4[i]).rgb;
	}
    vec3 sector4Average = sector4Sum * 0.04f;

    vec3 sector4VarianceSum = vec3(0.0f);
    for (int i = 0; i < 25; i++) {
		pixelVariance = (texture(inputTexture, fTexCoords.st + SECTOR_4[i]).rgb - sector4Average);
        pixelVariance *= pixelVariance;
        sector4VarianceSum += pixelVariance;
	}

    //choose sector with the lowest variance
    float sector1Score = length(sector1VarianceSum), sector2Score = length(sector2VarianceSum), sector3Score = length(sector3VarianceSum), sector4Score = length(sector4VarianceSum);

    vec4 scores = vec4(sector1Score, sector2Score, sector3Score, sector4Score);
    vec4 selection = step(scores, scores.xxxx);
    selection /= dot(selection, vec4(1.0));

    vec3 finalValue = selection.x * sector1Average + selection.y * sector2Average + selection.z * sector3Average + selection.w * sector4Average;
    
	//output colour
	fColour = vec4(finalValue, 1.0f);
}
