#include "player.h"

player::player() {
	playerX = 0;
	playerY = 0;
	playerZ = 0;
	playerChunkX = 0;
	playerChunkY = 0;
}

void player::updatePlayerPosition(float pX, float pY, float pZ) {
	playerX = pX;
	playerY = pY;
	playerZ = pZ;

	//podziel CHUNK_LENGTH
	playerChunkX = static_cast<int>(floor(pX * CHUNK_LENGTH_RECIPROCAL));
	playerChunkY = static_cast<int>(floor(pY * CHUNK_LENGTH_RECIPROCAL));
}

player::AABB player::getPlayerAABB() const {
	return {
		glm::vec3(playerX - 0.5, playerY - 0.5, playerZ - 2),
		glm::vec3(playerX + 0.5, playerY + 0.5, playerZ)
	};
}

glm::vec2 player::getPlayerChunk() const {
	return glm::vec2(playerChunkX, playerChunkY);
}

glm::vec3 player::getPlayerPosition() const {
	return glm::vec3(playerX, playerY, playerZ);
}