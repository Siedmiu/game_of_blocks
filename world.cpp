#include "world.h"
#include <iostream>
#include <chrono>
//#include <thread>

world::world(player& playerOne, camera& cam) : playerOne(playerOne), cam(cam) {}

//old AABB
/*
world::overlapInfoTruncation world::overlapAABBtruncation(const player::Aabb& playerAABB, const player::Aabb& blockAABB) {
    overlapInfoTruncation info;

	if (playerAABB.max.x < blockAABB.min.x || playerAABB.min.x > blockAABB.max.x ||
		playerAABB.max.y < blockAABB.min.y || playerAABB.min.y > blockAABB.max.y ||
		playerAABB.max.z < blockAABB.min.z || playerAABB.min.z > blockAABB.max.z) {
		info.isOverlapping = false;
		return info;
	}
	
	if (playerAABB.max.x - blockAABB.min.x < blockAABB.max.x - playerAABB.min.x) info.overlapX = overlapInfoTruncation::DimensionOverlap::negative; else info.overlapX = overlapInfoTruncation::DimensionOverlap::positive;
	if (playerAABB.max.y - blockAABB.min.y < blockAABB.max.y - playerAABB.min.y) info.overlapY = overlapInfoTruncation::DimensionOverlap::negative; else info.overlapY = overlapInfoTruncation::DimensionOverlap::positive;
	if (playerAABB.max.z - blockAABB.min.z < blockAABB.max.z - playerAABB.min.z) info.overlapZ = overlapInfoTruncation::DimensionOverlap::negative; else info.overlapZ = overlapInfoTruncation::DimensionOverlap::positive;

	return info;
}

world::overlapInfo world::overlapAABB(const player::Aabb& playerAABB, const player::Aabb& blockAABB) {
    overlapInfo info = { true, 0.0f, 0.0f, 0.0f };

    if (playerAABB.max.x < blockAABB.min.x || playerAABB.min.x > blockAABB.max.x) info.isOverlapping = false;
    else info.overlapX = std::min(playerAABB.max.x - blockAABB.min.x, blockAABB.max.x - playerAABB.min.x);

    if (playerAABB.max.y < blockAABB.min.y || playerAABB.min.y > blockAABB.max.y) info.isOverlapping = false;
    else info.overlapY = std::min(playerAABB.max.y - blockAABB.min.y, blockAABB.max.y - playerAABB.min.y);

    if (playerAABB.max.z < blockAABB.min.z || playerAABB.min.z > blockAABB.max.z) info.isOverlapping = false;
    else info.overlapZ = std::min(playerAABB.max.z - blockAABB.min.z, blockAABB.max.z - playerAABB.min.z);

    return info;
}

void world::AABBcolisionDetection() {
	player::Aabb playerAABB = playerOne.getPlayerAABB();
	player::Aabb blockAABB{};
	playerPosition = playerOne.getPlayerPosition();
	const float checkRadius = 0.5f, PLAYERHEIGHT = player::PLAYERHEIGHT, playerWidthHalf = player::PLAYERWIDTH * 0.5f;
	int blockX, blockY, blockChunkX, blockChunkY;
	blockChunkX = playerPosition.x * CHUNK_LENGTH_RECIPROCAL;
	blockChunkY = playerPosition.y * CHUNK_LENGTH_RECIPROCAL;
	overlapInfoTruncation::DimensionOverlap xOverlap{}, yOverlap{}, zOverlap{};

	for (int x = (int)(playerPosition.x - checkRadius - playerWidthHalf); x <= (int)(playerPosition.x + checkRadius + playerWidthHalf); x++) {
		for (int y = (int)(playerPosition.y - checkRadius - playerWidthHalf); y <= (int)(playerPosition.y + checkRadius + playerWidthHalf); y++) {
			for (int z = (int)(playerPosition.z - checkRadius - PLAYERHEIGHT); z <= (ceil)(playerPosition.z + checkRadius); z++) {
				if (z < 0 || z >= CHUNK_HEIGHT) continue;
				//if (xOverlap == overlapInfoTruncation::DimensionOverlap::none || yOverlap == overlapInfoTruncation::DimensionOverlap::none || zOverlap == overlapInfoTruncation::DimensionOverlap::none) break;
				blockX = x - blockChunkX * CHUNK_LENGTH;
				blockY = y - blockChunkY * CHUNK_LENGTH;

				if (blockX < 0) {
					blockX += CHUNK_LENGTH;
					blockChunkX -= 1;
				}
				else if (blockX >= CHUNK_LENGTH) {
					blockX -= CHUNK_LENGTH;
					blockChunkX += 1;
				}

				if (blockY < 0) {
					blockY += CHUNK_LENGTH;
					blockChunkY -= 1;
				}
				else if (blockY >= CHUNK_LENGTH) {
					blockY -= CHUNK_LENGTH;
					blockChunkY += 1;
				}

				uint8_t blockZ = z;

				uint8_t blockType = getBlock(blockChunkX, blockChunkY, (uint8_t)blockX, (uint8_t)blockY, blockZ);

				if (blockType != 32) {
					blockAABB.min.x = x;
					blockAABB.min.y = y;
					blockAABB.min.z = z;
					blockAABB.max.x = x + 1.0f;
					blockAABB.max.y = y + 1.0f;
					blockAABB.max.z = z + 1.0f;

					//coordinates method
					///*
					overlapInfo overlapInfo = overlapAABB(playerAABB, blockAABB);

					if (overlapInfo.isOverlapping) {
						float minOverlap = std::min({ overlapInfo.overlapX, overlapInfo.overlapY, overlapInfo.overlapZ });

						//determine the push back
						if (minOverlap == overlapInfo.overlapZ && z < playerPosition.z - player::PLAYERHEIGHT) {
							moveZ = overlapInfo.overlapZ;
						} else
						if (minOverlap == overlapInfo.overlapX) {
							moveX = (playerAABB.max.x - blockAABB.min.x < blockAABB.max.x - playerAABB.min.x) ? -overlapInfo.overlapX : overlapInfo.overlapX;
						} else
						if (minOverlap == overlapInfo.overlapY) {
							moveY = (playerAABB.max.y - blockAABB.min.y < blockAABB.max.y - playerAABB.min.y) ? -overlapInfo.overlapY : overlapInfo.overlapY;
						}
					}
					// /

					//truncation method
					overlapInfoTruncation overlapInfoTruncation = overlapAABBtruncation(playerAABB, blockAABB);
					if (!overlapInfoTruncation.isOverlapping) continue;

					if (xOverlap != overlapInfoTruncation::DimensionOverlap::none) {
						if (overlapInfoTruncation.overlapX == overlapInfoTruncation::DimensionOverlap::positive) {
							if (xOverlap == overlapInfoTruncation::DimensionOverlap::negative) {
								xOverlap = overlapInfoTruncation::DimensionOverlap::none;
							}
							else {
								xOverlap = overlapInfoTruncation::DimensionOverlap::positive;
							}
						}
						else {
							if (xOverlap == overlapInfoTruncation::DimensionOverlap::positive) {
								xOverlap = overlapInfoTruncation::DimensionOverlap::none;
							}
							else {
								xOverlap = overlapInfoTruncation::DimensionOverlap::negative;
							}
						}
					}

					if (yOverlap != overlapInfoTruncation::DimensionOverlap::none) {
						if (overlapInfoTruncation.overlapY == overlapInfoTruncation::DimensionOverlap::positive) {
							if (yOverlap == overlapInfoTruncation::DimensionOverlap::negative) {
								yOverlap = overlapInfoTruncation::DimensionOverlap::none;
							}
							else {
								yOverlap = overlapInfoTruncation::DimensionOverlap::positive;
							}
						}
						else {
							if (yOverlap == overlapInfoTruncation::DimensionOverlap::positive) {
								yOverlap = overlapInfoTruncation::DimensionOverlap::none;
							}
							else {
								yOverlap = overlapInfoTruncation::DimensionOverlap::negative;
							}
						}
					}

					if (zOverlap != overlapInfoTruncation::DimensionOverlap::none) {
						if (overlapInfoTruncation.overlapZ == overlapInfoTruncation::DimensionOverlap::positive) {
							if (zOverlap == overlapInfoTruncation::DimensionOverlap::negative) {
								zOverlap = overlapInfoTruncation::DimensionOverlap::none;
							}
							else {
								zOverlap = overlapInfoTruncation::DimensionOverlap::positive;
							}
						}
						else {
							if (zOverlap == overlapInfoTruncation::DimensionOverlap::positive) {
								zOverlap = overlapInfoTruncation::DimensionOverlap::none;
							}
							else {
								zOverlap = overlapInfoTruncation::DimensionOverlap::negative;
							}
						}
					}
				}
			}
		}
	}

	float moveX = 0.0, moveY = 0.0, moveZ = 0.0;

	if (xOverlap == overlapInfoTruncation::DimensionOverlap::positive) {
		moveX = ceil(playerPosition.x) - playerPosition.x - playerWidthHalf;
	} else if (xOverlap == overlapInfoTruncation::DimensionOverlap::negative) {
		moveX = floor(playerPosition.x) - playerPosition.x + playerWidthHalf;
	}

	if (yOverlap == overlapInfoTruncation::DimensionOverlap::positive) {
		moveY = ceil(playerPosition.y) - playerPosition.y - playerWidthHalf;
	}
	else if (yOverlap == overlapInfoTruncation::DimensionOverlap::negative) {
		moveY = floor(playerPosition.y) - playerPosition.y + playerWidthHalf;
	}

	if (zOverlap == overlapInfoTruncation::DimensionOverlap::positive) {
		moveZ = ceil(playerPosition.z) - playerPosition.z;
	}
	else if (zOverlap == overlapInfoTruncation::DimensionOverlap::negative) {
		moveZ = floor(playerPosition.z) - playerPosition.z;
	}

	onTheFloor = (moveZ) ? true : false;

	//openGL has different camera axis coordinates
	glm::vec3 move = glm::vec3(moveX, moveZ, moveY);
	cam.moveCamera(move);

}
*/

void world::updatePlayerPosition() {
	playerPosition = playerOne.getPlayerPosition();
	playerVelocity = playerOne.getPlayerVelocity();
	playerChunkX = int(playerPosition.x * CHUNK_LENGTH_RECIPROCAL);
	playerChunkY = int(playerPosition.y * CHUNK_LENGTH_RECIPROCAL);
}

world::overlapInfo world::sweptAABBcolisonCheckInfo(int x, int y, int z) const {

	world::overlapInfo overlapInfo;

	player::Aabb blockAABB{};
	blockAABB.min.x = float(x - playerOne.PLAYERWIDTH * 0.5);
	blockAABB.min.y = float(y - playerOne.PLAYERWIDTH * 0.5);
	blockAABB.min.z = float(z - playerOne.PLAYERHEIGHT * 0.5);
	blockAABB.max.x = float(x + BLOCK_SIZE + playerOne.PLAYERWIDTH * 0.5);
	blockAABB.max.y = float(y + BLOCK_SIZE + playerOne.PLAYERWIDTH * 0.5);
	blockAABB.max.z = float(z + BLOCK_SIZE + playerOne.PLAYERHEIGHT * 0.5);

	//determine the colision using playerVelocity and playerPosition
	float t_min = -FLT_MAX;
	float t_max = FLT_MAX;

	//maybe faster one w/o division

	if (playerVelocity.x != 0.0f) {
		float inverseVelocity_x = 1.0f / playerVelocity.x;
		float t1 = (blockAABB.min.x - playerPosition.x) * inverseVelocity_x;
		float t2 = (blockAABB.max.x - playerPosition.x) * inverseVelocity_x;

		t_min = std::max(t_min, std::min(t1, t2));
		t_max = std::min(t_max, std::max(t1, t2));
	}

	if (playerVelocity.y != 0.0f) {
		float inverseVelocity_y = 1.0f / playerVelocity.y;
		float t1 = (blockAABB.min.y - playerPosition.y) * inverseVelocity_y;
		float t2 = (blockAABB.max.y - playerPosition.y) * inverseVelocity_y;

		t_min = std::max(t_min, std::min(t1, t2));
		t_max = std::min(t_max, std::max(t1, t2));
	}

	if (playerVelocity.z != 0.0f) {
		float inverseVelocity_z = 1.0f / playerVelocity.z;
		float t1 = (blockAABB.min.z - playerPosition.z) * inverseVelocity_z;
		float t2 = (blockAABB.max.z - playerPosition.z) * inverseVelocity_z;

		t_min = std::max(t_min, std::min(t1, t2));
		t_max = std::min(t_max, std::max(t1, t2));
	}

	if (t_min <= t_max && t_min >= 0.0f) {
		overlapInfo.isOverlaping = true;
		overlapInfo.timeToColisoinE_4 = (unsigned short int)(t_min * 10000);

		glm::vec3 overlapDistance{};
		if (playerVelocity.x > 0.0f)
			overlapInfo.overlapDistance.x = blockAABB.max.x - playerPosition.x;
		else if (playerVelocity.x < 0.0f)
			overlapInfo.overlapDistance.x = blockAABB.min.x - playerPosition.x;

		if (playerVelocity.y > 0.0f)
			overlapInfo.overlapDistance.y = blockAABB.max.y - playerPosition.y;
		else if (playerVelocity.y < 0.0f)
			overlapInfo.overlapDistance.y = blockAABB.min.y - playerPosition.y;

		if (playerVelocity.z > 0.0f)
			overlapInfo.overlapDistance.z = blockAABB.max.z - playerPosition.z;
		else if (playerVelocity.z < 0.0f)
			overlapInfo.overlapDistance.z = blockAABB.min.z - playerPosition.z;
	}
	
	return overlapInfo;
}

//swept aabb
// 1. Determine broad phase area:
//	 - player::getPlayerBroadAABB()
//	 - check for solid blocks
// 
// 2. Sweep:
//	- add player size to blocks
//	- determine overlap
//	- select smallest overlaping axi
//  - calculate intersection point
// 
// 2. Slide response:
//	- calculate slide response
//  - 3 sliding passes
//
// ? potencial to add deflection (bounce)
//
void world::sweptAABBcolisonCheck() {
	player::Aabb broadAABB = playerOne.getPlayerAABB();
	world::overlapInfo overlapNearest, overlapTemp;
	uint8_t blockType = 32;
	int blockX = 0, blockY = 0, blockChunkX = 0, blockChunkY = 0;

	for (int z = (int)broadAABB.min.z; z <= (int)broadAABB.max.z; z++) {
		if (z < 0 || z >= CHUNK_HEIGHT) continue;

		for (int y = (int)broadAABB.min.y; y <= (int)broadAABB.max.y; y++) {
			for (int x = (int)broadAABB.min.x; x <= (int)broadAABB.max.x; x++) {
				blockType = getBlockWorldspace(x, y, z);
				if (blockType == 32) continue;

				overlapTemp = sweptAABBcolisonCheckInfo(x, y, z);
				if (!overlapTemp.isOverlaping) continue;

				if (overlapTemp.timeToColisoinE_4 < overlapNearest.timeToColisoinE_4) overlapNearest = overlapTemp;
			}
		}
	}

	if (!overlapNearest.isOverlaping) return;

	playerOne.updatePlayerVelocity(overlapNearest.overlapDistance.x, overlapNearest.overlapDistance.y, overlapNearest.overlapDistance.z);
}

void world::createChunks() {
	if (playerChunkX == lastPlayerChunkX && playerChunkY == lastPlayerChunkY) return;
	lastPlayerChunkX = playerChunkX;
	lastPlayerChunkY = playerChunkY;

	std::unordered_set<std::pair<int, int>, pairHash> requiredChunks;
	//tolerance chunks to reduce loading when player moves across chunk borders
	std::unordered_set<std::pair<int, int>, pairHash> toleranceChunks;

	//determine the circle of chunks
	for (int dx = 0; dx <= RENDER_DISTANCE_CHUNKS; ++dx) {
		for (int dy = 0; dy <= RENDER_DISTANCE_CHUNKS; ++dy) {
			if (dx * dx + dy * dy <= RENDER_DISTANCE_CHUNKS * RENDER_DISTANCE_CHUNKS) {

				if (dx != 0 || dy != 0) {
					requiredChunks.emplace(playerChunkX + dx, playerChunkY + dy);
					requiredChunks.emplace(playerChunkX - dx, playerChunkY + dy);
					requiredChunks.emplace(playerChunkX + dx, playerChunkY - dy);
					requiredChunks.emplace(playerChunkX - dx, playerChunkY - dy);

					toleranceChunks.emplace(playerChunkX + dx, playerChunkY + dy);
					toleranceChunks.emplace(playerChunkX - dx, playerChunkY + dy);
					toleranceChunks.emplace(playerChunkX + dx, playerChunkY - dy);
					toleranceChunks.emplace(playerChunkX - dx, playerChunkY - dy);
				} else if (dx != 0 || dy == 0) {
					requiredChunks.emplace(playerChunkX + dx, playerChunkY);
					requiredChunks.emplace(playerChunkX - dx, playerChunkY);

					toleranceChunks.emplace(playerChunkX + dx, playerChunkY);
					toleranceChunks.emplace(playerChunkX - dx, playerChunkY);
				} else if (dx == 0 || dy != 0) {
					requiredChunks.emplace(playerChunkX, playerChunkY + dy);
					requiredChunks.emplace(playerChunkX, playerChunkY - dy);

					toleranceChunks.emplace(playerChunkX, playerChunkY + dy);
					toleranceChunks.emplace(playerChunkX, playerChunkY - dy);
				} else if (dx == 0 || dy == 0) {
					requiredChunks.emplace(playerChunkX, playerChunkY);

					toleranceChunks.emplace(playerChunkX, playerChunkY);
				}

			} else if (dx * dx + dy * dy <= (RENDER_DISTANCE_CHUNKS + 1) * (RENDER_DISTANCE_CHUNKS + 1)) {
				toleranceChunks.emplace(playerChunkX + dx, playerChunkY + dy);
				toleranceChunks.emplace(playerChunkX - dx, playerChunkY + dy);
				toleranceChunks.emplace(playerChunkX + dx, playerChunkY - dy);
				toleranceChunks.emplace(playerChunkX - dx, playerChunkY - dy);
			}	
		}
	}


	/*std::vector<std::thread> threads;
	for (const auto& chunk : requiredChunks) {
		if (existingChunks.find(chunk) == existingChunks.end()) {
			threads.emplace_back(&world::newChunk, this, chunk.first, chunk.second);
		}
	}
	for (auto& thread : threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}*/
	
	
	for (const auto& chunk : requiredChunks) {
		if (existingChunks.find(chunk) == existingChunks.end()) {
			newChunk(chunk.first, chunk.second);
		}
	}


	auto tempExistingChunks = existingChunks;

	existingChunks = std::move(requiredChunks);

	for (auto it = tempExistingChunks.begin(); it != tempExistingChunks.end();) {
		if (toleranceChunks.find(*it) == toleranceChunks.end()) {
			deleteChunk(it->first, it->second);
			it = tempExistingChunks.erase(it);
		}
		else {
			++it;
		}
	}

	for (const auto& chunk : tempExistingChunks) {
		if (toleranceChunks.find(chunk) != toleranceChunks.end() &&
			existingChunks.find(chunk) == existingChunks.end()) {
			existingChunks.emplace(chunk);
		}
	}
}

//nie da sie sprawdzic chunkow obok jak jeszcze nie istnieja, poprawic dla nowego generatora szumu
void world::generateChunkMesh(chunk& c) {
	std::vector<float> vertices;
	glm::vec3 blockPosition;
	uint8_t blockID;
	bool back = true, front = true, left = true, right = true, bottom = true, top = true;

	for (uint8_t cx = 0; cx < CHUNK_LENGTH; cx++) {
		for (uint8_t cy = 0; cy < CHUNK_LENGTH; cy++) {
			for (uint8_t cz = 0; cz < CHUNK_HEIGHT; cz++) {
				blockID = c.chunkBlockData[get3dCoord(cx, cy, cz)];
				if (blockID == 32) continue; //skip air
				blockPosition = glm::vec3(cx, cz, cy);

				//check neighbours
				if (cx != 0) if (c.chunkBlockData[get3dCoord(cx - 1, cy, cz)] != AIR_ID) {
					left = false;
				};
				if (cx != CHUNK_LENGTH - 1) if (c.chunkBlockData[get3dCoord(cx + 1, cy, cz)] != AIR_ID) {
					right = false;
				};
				if (cy != 0) if (c.chunkBlockData[get3dCoord(cx, cy - 1, cz)] != AIR_ID) {
					back = false;
				};
				if (cy != CHUNK_LENGTH - 1) if (c.chunkBlockData[get3dCoord(cx, cy + 1, cz)] != AIR_ID) {
					front = false;
				};
				if (cz != 0) if (c.chunkBlockData[get3dCoord(cx, cy, cz - 1)] != AIR_ID) {
					bottom = false;
				};
				if (cz == 0) {
					bottom = false;
				};
				if (cz != CHUNK_HEIGHT - 1) if (c.chunkBlockData[get3dCoord(cx, cy, cz + 1)] != AIR_ID) {
					top = false;
				};

				//store 6 floats in 1 float
				//5 bits for x; 5 bits for y; 8 bits for z; 5 bit for texID; 3 bits for faceDirection; 26 bits total, 6 unoccupied
				//max x,y = 32; max z = 256; max texID = 32, max faceDirection = 8 (6 needed);
				//0b 00000 00000000 00000 00000 000 ------
				//
				union floatToUli {
					float floatData;
					unsigned long int uliData;
				};
				floatToUli vertexData{};
				unsigned long int container{}; //same bit length as a float

				if (back) {
					vertexData.uliData = 0;
					container = 0;

					//blockPosition.x
					container = blockPosition.x;
					container = container << (sizeof(float) * 8 - 5); //first 5
					vertexData.uliData |= container;

					//blockPosition.y is height
					container = blockPosition.y;
					container = container << (sizeof(float) * 8 - 5 - 8); //6 to 13
					vertexData.uliData |= container;

					//blockPosition.z
					container = blockPosition.z;
					container = container << (sizeof(float) * 8 - 5 - 8 - 5); // 14 to 18
					vertexData.uliData |= container;

					//texture ID
					container = static_cast<unsigned long int>(blockID);
					container = container << (sizeof(float) * 8 - 5 - 8 - 5 - 5); //19 to 23
					vertexData.uliData |= container;

					//face Direction
					container = static_cast<unsigned long int>(FACE_DIRECTION_BACK);
					container = container << (sizeof(float) * 8 - 5 - 8 - 5 - 5 - 3); //24 to 26
					vertexData.uliData |= container;

					vertices.push_back(vertexData.floatData);
				}
				if (front) {
					vertexData.uliData = 0;
					container = 0;

					container = blockPosition.x;
					container = container << (sizeof(float) * 8 - 5);
					vertexData.uliData |= container;

					container = blockPosition.y;
					container = container << (sizeof(float) * 8 - 13);
					vertexData.uliData |= container;

					container = blockPosition.z;
					container = container << (sizeof(float) * 8 - 18);
					vertexData.uliData |= container;

					container = static_cast<unsigned long int>(blockID);
					container = container << (sizeof(float) * 8 - 23);
					vertexData.uliData |= container;

					//face Direction
					container = static_cast<unsigned long int>(FACE_DIRECTION_FRONT);
					container = container << (sizeof(float) * 8 - 26);
					vertexData.uliData |= container;

					vertices.push_back(vertexData.floatData);
				}
				if (left) {
					vertexData.uliData = 0;
					container = 0;

					container = blockPosition.x;
					container = container << (sizeof(float) * 8 - 5);
					vertexData.uliData |= container;

					container = blockPosition.y;
					container = container << (sizeof(float) * 8 - 13);
					vertexData.uliData |= container;

					container = blockPosition.z;
					container = container << (sizeof(float) * 8 - 18);
					vertexData.uliData |= container;

					container = static_cast<unsigned long int>(blockID);
					container = container << (sizeof(float) * 8 - 23);
					vertexData.uliData |= container;

					//face Direction
					container = static_cast<unsigned long int>(FACE_DIRECTION_LEFT);
					container = container << (sizeof(float) * 8 - 26);
					vertexData.uliData |= container;

					vertices.push_back(vertexData.floatData);
				}
				if (right) {
					vertexData.uliData = 0;
					container = 0;

					container = blockPosition.x;
					container = container << (sizeof(float) * 8 - 5);
					vertexData.uliData |= container;

					container = blockPosition.y;
					container = container << (sizeof(float) * 8 - 13);
					vertexData.uliData |= container;

					container = blockPosition.z;
					container = container << (sizeof(float) * 8 - 18);
					vertexData.uliData |= container;

					container = static_cast<unsigned long int>(blockID);
					container = container << (sizeof(float) * 8 - 23);
					vertexData.uliData |= container;

					//face Direction
					container = static_cast<unsigned long int>(FACE_DIRECTION_RIGHT);
					container = container << (sizeof(float) * 8 - 26);
					vertexData.uliData |= container;

					vertices.push_back(vertexData.floatData);
				}
				if (bottom) {
					vertexData.uliData = 0;
					container = 0;

					container = blockPosition.x;
					container = container << (sizeof(float) * 8 - 5);
					vertexData.uliData |= container;

					container = blockPosition.y;
					container = container << (sizeof(float) * 8 - 13);
					vertexData.uliData |= container;

					container = blockPosition.z;
					container = container << (sizeof(float) * 8 - 18);
					vertexData.uliData |= container;

					container = static_cast<unsigned long int>(blockID);
					container = container << (sizeof(float) * 8 - 23);
					vertexData.uliData |= container;

					//face Direction
					container = static_cast<unsigned long int>(FACE_DIRECTION_BOTTOM);
					container = container << (sizeof(float) * 8 - 26);
					vertexData.uliData |= container;

					vertices.push_back(vertexData.floatData);
				}
				if (top) {
					vertexData.uliData = 0;
					container = 0;

					container = blockPosition.x;
					container = container << (sizeof(float) * 8 - 5);
					vertexData.uliData |= container;

					container = blockPosition.y;
					container = container << (sizeof(float) * 8 - 13);
					vertexData.uliData |= container;

					container = blockPosition.z;
					container = container << (sizeof(float) * 8 - 18);
					vertexData.uliData |= container;

					container = static_cast<unsigned long int>(blockID);
					container = container << (sizeof(float) * 8 - 23);
					vertexData.uliData |= container;

					//face Direction
					container = static_cast<unsigned long int>(FACE_DIRECTION_TOP);
					container = container << (sizeof(float) * 8 - 26);
					vertexData.uliData |= container;

					vertices.push_back(vertexData.floatData);
				}
				back = true;
				front = true;
				left = true;
				right = true;
				bottom = true;
				top = true;
			}
		}
	}

	//VAO for a draw command
	glCreateVertexArrays(1, &c.VAO);
	glBindVertexArray(c.VAO);

	//SSBO holds compressed vertex data
	glCreateBuffers(1, &c.SSBO);
	glNamedBufferStorage(c.SSBO, vertices.size() * sizeof(float), vertices.data(), 0);

	//bind point
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, c.SSBO);

	c.vertexCount = vertices.size();
	c.needsUpdate = false;
	c.notInFOV = false;
}

void world::newChunk(int x, int y) {
	//auto zamiast std::unordered_map<std::pair<int, int>, chunk, pairHash> chunks;
	//unique ptr heap;
	auto newChunk = std::make_unique<chunk>();
	newChunk->chunkX = x;
	newChunk->chunkY = y;
	newChunk->needsUpdate = true;
	newChunk->VAO = 0;
	newChunk->SSBO = 0;

	//set block data
	float noiseMap[CHUNK_LENGTH * CHUNK_LENGTH]{};
	noiseGenerator(x, y, noiseMap);

	for (uint8_t cx = 0; cx < CHUNK_LENGTH; cx++) {
		for (uint8_t cy = 0; cy < CHUNK_LENGTH; cy++) {

			uint8_t height = (uint8_t)(MIN_HEIGHT + noiseMap[get2dCoord(cx, cy)] * (CHUNK_HEIGHT - MIN_HEIGHT));
			if (height >= CHUNK_HEIGHT) {
				height = CHUNK_HEIGHT - 1;
			}
			else if (height < MIN_HEIGHT) height = MIN_HEIGHT;

			//uint8_t height = (cx % 4 || cy % 4) ? 2 : 90;
			
			for (uint8_t cz = 0; cz <= height - 2; ++cz) {
				newChunk->chunkBlockData[get3dCoord(cx, cy, cz)] = STONE_ID;
			}
			newChunk->chunkBlockData[get3dCoord(cx, cy, height - 1)] = DIRT_ID;
			newChunk->chunkBlockData[get3dCoord(cx, cy, height)] = GRASS_ID;
			for (uint8_t cz = height + 1; cz < CHUNK_HEIGHT; ++cz) {
				newChunk->chunkBlockData[get3dCoord(cx, cy, cz)] = AIR_ID;
			}
		}
	}

	generateChunkMesh(*newChunk);

	//std::lock_guard<std::mutex> lock(chunksMutex);

	chunks[{x, y}] = std::move(newChunk);

	//std::cout << "thread ended" << std::endl;
}

void world::deleteChunk(int x, int y) {
	auto it = chunks.find({ x, y });
	if (it != chunks.end()) {
		glDeleteVertexArrays(1, &(it->second->VAO));
		glDeleteBuffers(1, &(it->second->SSBO));
		chunks.erase(it);
	}
	existingChunks.erase({ x, y });
}

uint8_t world::getBlockWorldspace(int x, int y, int z) {
	int chunkX = int(x * CHUNK_LENGTH_RECIPROCAL);
	int chunkY = int(y * CHUNK_LENGTH_RECIPROCAL);

	int blockX = int(x - chunkX * CHUNK_LENGTH);
	int blockY = int(y - chunkY * CHUNK_LENGTH);

	if (blockX < 0) blockX += CHUNK_LENGTH - 1;
	if (blockY < 0) blockY += CHUNK_LENGTH - 1;
	
	return getBlock(chunkX, chunkY, blockX, blockY, z);
}

uint8_t world::getBlock(int chunkX, int chunkY, uint8_t x, uint8_t y, uint8_t z) {
	auto it = chunks.find({ chunkX, chunkY });
	if (it != chunks.end()) {
		return it->second->chunkBlockData[get3dCoord(x, y, z)];
	}
	return 0; //nie ma chunka
}

void world::setBlock(int chunkX, int chunkY, uint8_t x, uint8_t y, uint8_t z, uint8_t blockType) {
	auto it = chunks.find({ chunkX, chunkY });
	if (it != chunks.end()) {
		it->second->chunkBlockData[get3dCoord(x, y, z)] = blockType;
	}
}

//GENERATOR SZUMU---------------------------------------------
float world::cubicInterpolator(float a, float b, float weight) {
	return (float)((b - a) * (3.0 - weight * 2.0) * weight * weight + a);
}

float world::lerp(float a, float b, float weight) {
	//linear interpolation function
	return a + weight * (b - a);
}

float world::fade(float a) { //ttv/faide
	//input 0 to 1, output 0 to 1
	//smooths out the kinks of the gradients
	//smooth stepping function: 6x^5 -15x^4 + 10x^3
	return a * a * a * (a * (a * 6 - 15) + 10);
}

inline float world::fadeCheap(float a) {
	//input 0 to 1, output 0 to 1
	//smooths out the kinks of the gradients
	//smooth stepping function: 3x^2 - 2x^3
	return a * a * (3 - 2 * a);
}

std::pair<float, float> world::gradientRNGvec2(int i_x, int i_y) {
	//based on the coordinates and the seed value calculates
	//the gradient unit vectors on the grid edges
	const unsigned int widthInt = 8 * sizeof(int);
	const unsigned int halfwidthInt = int(widthInt * 0.5);
	unsigned int a = SEED ^ i_x, b = SEED ^ i_y;

	//szahermaher z bitami na podstawie seed
	a *= 3284157443;
	b ^= (a << halfwidthInt) | (a >> (widthInt - halfwidthInt));
	b *= 1911520717;
	a ^= (b << halfwidthInt) | (b >> (widthInt - halfwidthInt));
	a *= 2048419325;

	//~0u = 0xFFFFFFFF
	float random = (float)(a * (3.14159265 / ~(~0u >> 1))); //[0, 2*Pi]

	// -1 to 1, unit vector length = sqrt(2)
	std::pair<float, float> p;
	p.first = sin(random);
	p.second = cos(random);

	return p;
};

float world::gradientRNG(float i_x, float i_y) {
	//random value at point (0 to 1) based on float values
	unsigned int ix, iy;
	std::memcpy(&ix, &i_x, sizeof(float));
	std::memcpy(&iy, &i_y, sizeof(float));

	const unsigned int widthInt = 8 * sizeof(int);
	const unsigned int halfwidthInt = int(widthInt * 0.5);
	unsigned int a = SEED ^ ix, b = SEED ^ iy;

	//szahermaher z bitami na podstawie seed
	a *= 3284157443;
	b ^= (a << halfwidthInt) | (a >> (widthInt - halfwidthInt));
	b *= 1911520717;
	a ^= (b << halfwidthInt) | (b >> (widthInt - halfwidthInt));
	a *= 2048419325;

	//~0u = 0xFFFFFFFF
	float random = (float)(a * (0.5f / ~(~0u >> 1)));

	return random;
};

float world::scalarProductNormalized(float dx, float dy, std::pair<float, float> gradient) {
	//gradient is a unit vector
	// (dx,dx) is a position vector, dx,dy = [0,1]
	//dot product range is [-2,2]
	//result is normalized to [0, 1] range
	return ((dx * gradient.first + dy * gradient.second) + 2) * 0.25;
}

void world::perlinNoiseOctave(int chunkX, int chunkY, float* perlinNoise, float frequency, float amplitude) {
	//grid corner coordinates (spiral)
	int x0 = chunkX;
	int y0 = chunkY;
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	//punkty mapy
	float map_up_left, map_up_right, map_down_right, map_down_left;
	float bottom_pair, top_pair;

	//gradient na krawedziach
	std::pair<float, float> gradient_up_left = gradientRNGvec2(x0, y1);
	std::pair<float, float> gradient_up_right = gradientRNGvec2(x1, y1);
	std::pair<float, float> gradient_down_right = gradientRNGvec2(x1, y0);
	std::pair<float, float> gradient_down_left = gradientRNGvec2(x0, y0);

	//wagi interpolacji
	float wx, wy;

	for (uint8_t x = 0; x < CHUNK_LENGTH; x++) {
		wx = x * CHUNK_LENGTH_RECIPROCAL;

		for (uint8_t y = 0; y < CHUNK_LENGTH; y++) {
			wy = y * CHUNK_LENGTH_RECIPROCAL;

			//obliczenia krawedzi
			map_up_left = scalarProductNormalized(wx, wy, gradient_up_left);
			map_up_right = scalarProductNormalized(wx, wy, gradient_up_right);
			map_down_right = scalarProductNormalized(wx, wy, gradient_down_right);
			map_down_left = scalarProductNormalized(wx, wy, gradient_down_left);

			map_up_left = fade(map_up_left);
			map_up_right = fade(map_up_right);
			map_down_right = fade(map_down_right);
			map_down_left = fade(map_down_left);

			//interpolate horizontally
			top_pair = lerp(map_up_left, map_up_right, wx);
			bottom_pair = lerp(map_down_left, map_down_right, wx);

			//interpolate vertically
			perlinNoise[get2dCoord(x, y)] += lerp(bottom_pair, top_pair, wy) * amplitude;
		}
	}
}

void world::polynomialNoiseGridCell(float* gridCellnoiseMap, float gridX0, float gridY0, int gridCellLength) {
	float gridX1 = gridX0 + gridCellLength;
	float gridY1 = gridY0 + gridCellLength;

	//grid cell edge values
	float a = gradientRNG(gridX0, gridY0);
	float b = gradientRNG(gridX1, gridY0);
	float c = gradientRNG(gridX0, gridY1);
	float d = gradientRNG(gridX1, gridY1);

	float gridCellSizeReciprocal = 1.0f / gridCellLength;

	int startX = ((int)(gridX0) % CHUNK_LENGTH + CHUNK_LENGTH) % CHUNK_LENGTH;
	int startY = ((int)(gridY0) % CHUNK_LENGTH + CHUNK_LENGTH) % CHUNK_LENGTH;

	//interpolation weights
	float wx, wy;

	for (uint8_t x = 0; x < gridCellLength; x++) {
		wx = x * gridCellSizeReciprocal;

		for (uint8_t y = 0; y < gridCellLength; y++) {
			wy = y * gridCellSizeReciprocal;

			gridCellnoiseMap[get2dCoord(startX + x, startY + y)] = polynomialNoiseSample(wx, wy, a, b, c, d);
		}
	}
}

inline float world::polynomialNoiseSample(float dx, float dy, float a, float b, float c, float d) {
	//should remain between 0 and 1
	return (
		a +
		(b - a) * fadeCheap(dx) +
		(c - a) * fadeCheap(dy) +
		(a - b - c + d) * fadeCheap(dx) * fadeCheap(dy)
		);
}

void world::noiseGenerator(int chunkX, int chunkY, float* noiseMap) {
	float amplitude = 1.0f;
	float totalAmplitude = 0.0f;
	int gridCellLength = CHUNK_LENGTH;  //will be chunk region later
	int numCellsPerRow = 1;

	float* tempNoiseMap = new float[CHUNK_LENGTH * CHUNK_LENGTH];
	float gridX0 = chunkX;
	float gridY0 = chunkY;

	for (unsigned int octave = 0; octave < OCTAVES; octave++) {
		std::fill(tempNoiseMap, tempNoiseMap + (CHUNK_LENGTH * CHUNK_LENGTH), 0.0f);

		//rotation to prevent domain alignment
		//for (int i = 0; i < octave; i++) {
		//	float tempX0 = x0;
		//	x0 = ROTATION_MATRIX[0][0] * x0 + ROTATION_MATRIX[0][1] * y0;
		//	y0 = ROTATION_MATRIX[1][0] * tempX0 + ROTATION_MATRIX[1][1] * y0;
		//}

		for (int gx = 0; gx < numCellsPerRow; gx++) {
			for (int gy = 0; gy < numCellsPerRow; gy++) {
				gridX0 = chunkX * CHUNK_LENGTH + gx * gridCellLength;
				gridY0 = chunkY * CHUNK_LENGTH + gy * gridCellLength;

				//Generate noise
				polynomialNoiseGridCell(tempNoiseMap, gridX0, gridY0, gridCellLength);
			}
		}

		//add octave's noise contribution
		for (uint8_t x = 0; x < CHUNK_LENGTH; x++) {
			for (uint8_t y = 0; y < CHUNK_LENGTH; y++) {
				noiseMap[get2dCoord(x, y)] += tempNoiseMap[get2dCoord(x, y)] * amplitude;
			}
		}

		totalAmplitude += amplitude;
		numCellsPerRow <<= 1;  //Next octave has 4x the cells (2x the frequency)
		gridCellLength >>= 1;  //Each grid cell is half the size
		//min size of a grid cell
		if (gridCellLength < 4) break;

		amplitude *= PERSISTANCE;
	}

	// Normalize the final noise map
	for (uint8_t x = 0; x < CHUNK_LENGTH; x++) {
		for (uint8_t y = 0; y < CHUNK_LENGTH; y++) {
			noiseMap[get2dCoord(x, y)] /= totalAmplitude;
		}
	}
	delete[] tempNoiseMap;
}

/*
	auto t1 = std::chrono::high_resolution_clock::now();
	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << duration << std::endl;
*/