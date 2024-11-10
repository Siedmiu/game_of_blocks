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

const unsigned short int CHUNK_HEIGHT = 100, CHUNK_LENGTH = 16;
const float CHUNK_LENGTH_RECIPROCAL = 1.0f / CHUNK_LENGTH;

class world {
private:
	//WORLD SETTINGS
	const unsigned short int RENDER_DISTANCE_CHUNKS = 1, BLOCK_SIZE = 1;
	static const unsigned short int CHUNK_VOLUME = CHUNK_HEIGHT * CHUNK_LENGTH * CHUNK_LENGTH;

	//GENERATION SETTINGS
	static const unsigned int SEED = 1234;
	const unsigned int OCTAVES = 1;
	const float PERSISTANCE = 0.5f;

	int playerChunkX = 0, playerChunkY = 0, lastPlayerChunkX = 1, lastPlayerChunkY = 1;
	glm::vec3 playerPosition{}, playerVelocity{};

	player& playerOne;
	camera& cam;

	const float blockVertices[180] = {
		//pos                //texture cords
		0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		0.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		// ^ back ^

		0.0f, 0.0f,  1.0f,  0.0f, 0.0f,
		1.0f, 0.0f,  1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
		1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
		0.0f,  1.0f,  1.0f,  0.0f, 1.0f,
		0.0f, 0.0f,  1.0f,  0.0f, 0.0f,
		// ^ front ^

		0.0f,  1.0f,  1.0f,  1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		0.0f, 0.0f,  1.0f,  0.0f, 0.0f,
		0.0f,  1.0f,  1.0f,  1.0f, 0.0f,
		// ^ left ^

		1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
		1.0f, 0.0f,  1.0f,  0.0f, 0.0f,
		// ^ right ^

		0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
		1.0f, 0.0f,  1.0f,  1.0f, 0.0f,
		1.0f, 0.0f,  1.0f,  1.0f, 0.0f,
		0.0f, 0.0f,  1.0f,  0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		// ^ bottom ^

		0.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		0.0f,  1.0f,  1.0f,  0.0f, 0.0f
		// ^ top ^
	};
	const unsigned int indices[6] = {
	0, 1, 3,   //first triangle
	1, 2, 3    //second triangle
	};

	struct chunk {
		int chunkX{}, chunkY{};
		uint8_t chunkBlockData[CHUNK_VOLUME]{};
		unsigned int VAO{}, VBO{}, EBO{}, indexCount{};
		bool needsUpdate = true, notInFOV = false;
	};

	struct pairHash {
		std::size_t operator()(const std::pair<int, int>& pair) const {
			//funkcja oblicza hash dla unordered_map aby adresowac chunki
			std::size_t h1 = std::hash<int>{}(pair.first);
			std::size_t h2 = std::hash<int>{}(pair.second);
			return h1 ^ (h2 << 1); //przesuniecie bitu i kombinacja haszy
		}
	};

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

	std::unordered_map<std::pair<int, int>, std::unique_ptr<chunk>, pairHash> chunks;
	std::unordered_set<std::pair<int, int>, pairHash> existingChunks;

	//overlapInfo overlapAABB(const player::Aabb& playerAABB, const player::Aabb& blockAABB);
	//overlapInfoTruncation overlapAABBtruncation(const player::Aabb& playerAABB, const player::Aabb& blockAABB);
	overlapInfo sweptAABBcolisonCheckInfo(int x, int y, int z) const;
	void generateChunkMesh(chunk& c);
	float cubicInterpolator(float a, float b, float weight);
	float lerp(float a, float b, float weight);
	float fade(float a);
	float scalarProduct(float dx, float dy, std::pair<float, float> gradient);
	std::pair<float, float> gradientRNG(int x, int y);
	void perlinNoiseOctave(int chunkX, int chunkY, float* perlinNoise, float frequency, float amplitude);
	void perlinNoiseGenerator(int chunkX, int chunkY, float* perlinNoise, unsigned int octaves, float persistence);

	uint8_t getBlockWorldspace(int X, int Y, int Z);
	uint8_t getBlock(int chunkX, int chunkY, uint8_t x, uint8_t y, uint8_t z);
	void setBlock(int chunkX, int chunkY, uint8_t x, uint8_t y, uint8_t z, uint8_t blockType);

	//inline podmienia kod tam gdzie uzywamy funkcji
	inline int get3dCoord(uint8_t x, uint8_t y, uint8_t z) const {
		return x + CHUNK_LENGTH * (y + CHUNK_LENGTH * z);
	}
	inline int get2dCoord(uint8_t x, uint8_t y) const {
		return x + CHUNK_LENGTH * y;
	}

public:
	world(player& playerOne, camera& cam);

	//alias chunk type
	using Chunk = chunk;

	void updatePlayerPosition();
	void sweptAABBcolisonCheck();
	//void AABBcolisionDetection();
	void createChunks();
	void newChunk(int x, int y);
	void deleteChunk(int x, int y);

	const std::unordered_map<std::pair<int, int>, std::unique_ptr<Chunk>, pairHash>& getChunks() const {
		return chunks;
	}
};