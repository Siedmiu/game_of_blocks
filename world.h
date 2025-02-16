#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <memory>

#include "player.h"
#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//max chunk height is 255
const unsigned short int CHUNK_HEIGHT = 255, CHUNK_LENGTH = 16;
const float CHUNK_LENGTH_RECIPROCAL = 1.0f / CHUNK_LENGTH;

class world {
private:
	//WORLD SETTINGS
	const unsigned short int RENDER_DISTANCE_CHUNKS = 30, BLOCK_SIZE = 1;
	static const unsigned int CHUNK_VOLUME = CHUNK_HEIGHT * CHUNK_LENGTH * CHUNK_LENGTH;
	static const unsigned short int REGION_LENGTH_CHUNKS = 32, REGION_RENDER_RADIUS = 6; //17
	const unsigned short int REGION_LENGTH = REGION_LENGTH_CHUNKS * CHUNK_LENGTH;

	//GENERATION SETTINGS
	static const unsigned int SEED = 1234;
	const unsigned int OCTAVES = 6;
	const float PERSISTANCE = 0.5f;
	const float STEEPNESS_FACTOR = 2.0f;
	const unsigned short int MIN_HEIGHT = 5;

	const float ROTATION_MATRIX[2][2] = {
	{4.0f / 5.0f, -3.0f / 5.0f},
	{3.0f / 5.0f, 4.0f / 5.0f}
	};

	//BLOCKS
	const uint8_t FACE_DIRECTION_BACK = 0, FACE_DIRECTION_FRONT = 1, FACE_DIRECTION_LEFT = 2, 
				  FACE_DIRECTION_RIGHT = 3, FACE_DIRECTION_BOTTOM = 4, FACE_DIRECTION_TOP = 5;

	const uint8_t GRASS_ID = 0, DIRT_ID = 1, STONE_ID = 2, AIR_ID = 32;

	/*
	const float blockVertices[120] = {
		//pos             //texture cords
		1.0f, 0.0f, 0.0f,  1.0f, 0.0f,  //bottom-right
		0.0f, 0.0f, 0.0f,  0.0f, 0.0f,  //bottom-left
		1.0f, 1.0f, 0.0f,  1.0f, 1.0f,  //top-right
		0.0f, 1.0f, 0.0f,  0.0f, 1.0f,  //top-left
		// ^ back ^

		0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  //bottom-left
		1.0f, 0.0f, 1.0f,  1.0f, 0.0f,  //bottom-right
		0.0f, 1.0f, 1.0f,  0.0f, 1.0f,  //top-left
		1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  //top-right
		// ^ front ^
		
		0.0f, 0.0f, 0.0f,  1.0f, 0.0f,  //bottom-back
		0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  //bottom-front
		0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  //top-back
		0.0f, 1.0f, 1.0f,  0.0f, 1.0f,  //top-front
		// ^ left ^

		1.0f, 0.0f, 1.0f,  0.0f, 0.0f,  //bottom-front
		1.0f, 0.0f, 0.0f,  1.0f, 0.0f,  //bottom-back
		1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  //top-front
		1.0f, 1.0f, 0.0f,  1.0f, 1.0f,  //top-back
		// ^ right ^

		0.0f, 0.0f, 0.0f,  0.0f, 1.0f,  //back-left
		1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  //back-right
		0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  //front-left
		1.0f, 0.0f, 1.0f,  1.0f, 0.0f,  //front-right
		// ^ bottom ^

		1.0f, 1.0f, 0.0f,  1.0f, 1.0f,  //back-right
		0.0f, 1.0f, 0.0f,  0.0f, 1.0f,  //back-left
		1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  //front-right
		0.0f, 1.0f, 1.0f,  0.0f, 0.0f   //front-left
		// ^ top ^
	};
	
	const unsigned int indices[6] = {
	0, 1, 3,   //first triangle
	1, 2, 3    //second triangle
	};*/

	//player data
	int playerChunkX = 0, playerChunkY = 0, lastPlayerChunkX = 1, lastPlayerChunkY = 1;
	glm::vec3 playerPosition{}, playerVelocity{};

	player& playerOne;
	camera& cam;

	//world data
	struct chunk {
		int chunkX{}, chunkY{};
		unsigned int VAO{}, SSBO{};
		unsigned int vertexCount = 0;
		bool needsUpdate = true, notInFOV = false;
		//2 bytes of free space here
		uint8_t chunkBlockData[CHUNK_VOLUME]{};
	};

	struct region {
		int regionX{}, regionY{};
		//regionHeightData = 1MB
		uint8_t regionHeightData[REGION_LENGTH_CHUNKS * REGION_LENGTH_CHUNKS * CHUNK_LENGTH * CHUNK_LENGTH] {};
	};

	struct pairHash {
		std::size_t operator()(const std::pair<int, int>& pair) const {
			//funkcja oblicza hash dla unordered_map aby adresowac chunki
			std::size_t h1 = std::hash<int>{}(pair.first);
			std::size_t h2 = std::hash<int>{}(pair.second);
			return h1 ^ (h2 << 1); //przesuniecie bitu i kombinacja haszy
		}
	};

	//region data can't be a unique pointer //WHY?! it was 4 months ago I dont remember :(
	std::unordered_map<std::pair<int, int>, std::unique_ptr<region>, pairHash> regions;
	std::unordered_set<std::pair<int, int>, pairHash> existingRegions;

	std::unordered_map<std::pair<int, int>, std::unique_ptr<chunk>, pairHash> chunks;
	std::unordered_set<std::pair<int, int>, pairHash> existingChunks;

	//colison
	/*
	struct overlapInfo {
		bool isOverlapping;
		float overlapX;
		float overlapY;
		float overlapZ;
	};

	struct overlapInfoTruncation {
		enum class DimensionOverlap { null, none, positive, negative };
		bool isOverlapping = true;
		DimensionOverlap overlapX = DimensionOverlap::null;
		DimensionOverlap overlapY = DimensionOverlap::null;
		DimensionOverlap overlapZ = DimensionOverlap::null;
	};*/

	struct overlapInfo {
		bool isOverlaping = false;
		unsigned short int timeToColisoinE_4 = USHRT_MAX; // jesli za malo dokladnosci to jeszcze jeden bajt wolny
		glm::vec3 overlapDistance{};
	};

	//overlapInfo overlapAABB(const player::Aabb& playerAABB, const player::Aabb& blockAABB);
	//overlapInfoTruncation overlapAABBtruncation(const player::Aabb& playerAABB, const player::Aabb& blockAABB);
	overlapInfo sweptAABBcolisonCheckInfo(int x, int y, int z) const;
	void generateChunkMesh(chunk& c);

	void generateRegions();

	//perlin noise generationm
	float cubicInterpolator(float a, float b, float weight);
	float lerp(float a, float b, float weight);
	float fade(float a);
	inline float fadeCheap(float a);
	inline float fadeCheapDerivative(float a);
	float scalarProductNormalized(float dx, float dy, std::pair<float, float> gradient);
	std::pair<float, float> gradientRNGvec2(int x, int y);
	float gradientRNG(float i_x, float i_y);

	inline glm::vec2 polynomialNoiseSample(float dx, float dy, float a, float b, float c, float d);
	void perlinNoiseOctave(int chunkX, int chunkY, float* perlinNoise, float frequency, float amplitude);
	void polynomialNoiseGridCell(float* gridCellnoiseMap, float* gridCellSteepnessMap, float x0, float y0, int gridCellLength, float amplitude, unsigned int octave);
	void noiseGenerator(int regionX, int regionY, uint8_t* noiseMap);

	void newChunk(int x, int y);
	void deleteChunk(int x, int y);

	uint8_t getBlockWorldspace(int X, int Y, int Z);
	uint8_t getBlock(int chunkX, int chunkY, uint8_t x, uint8_t y, uint8_t z);
	void setBlock(int chunkX, int chunkY, uint8_t x, uint8_t y, uint8_t z, uint8_t blockType);

	//inline podmienia kod tam gdzie uzywamy funkcji
	inline int get3dCoordChunk(uint8_t x, uint8_t y, uint8_t z) const {
		return x + CHUNK_LENGTH * (y + CHUNK_LENGTH * z);
	}
	inline int get2dCoordChunk(uint8_t x, uint8_t y) const {
		return x + CHUNK_LENGTH * y;
	}
	inline int get2dCoordRegion(unsigned short int x, unsigned short int y) const {
		return x + REGION_LENGTH * y;
	}

public:
	world(player& playerOne, camera& cam);

	//alias chunk type
	using Chunk = chunk;

	void updatePlayerPosition();
	void sweptAABBcolisonCheck();
	//void AABBcolisionDetection();
	void createChunks();

	const std::unordered_map<std::pair<int, int>, std::unique_ptr<Chunk>, pairHash>& getChunks() const {
		return chunks;
	}
};