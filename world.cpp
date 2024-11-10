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
	auto t1 = std::chrono::high_resolution_clock::now();

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

	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << duration << std::endl;
}

//nie da sie sprawdzic chunkow obok jak jeszcze nie istnieja, poprawic dla nowego generatora szumu
void world::generateChunkMesh(chunk& c) {
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	c.indexCount = 0;
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
				if (cx != 0) if (c.chunkBlockData[get3dCoord(cx - 1, cy, cz)] != 32) {
					left = false;
				};
				if (cx != CHUNK_LENGTH - 1) if (c.chunkBlockData[get3dCoord(cx + 1, cy, cz)] != 32) {
					right = false;
				};
				if (cy != 0) if (c.chunkBlockData[get3dCoord(cx, cy - 1, cz)] != 32) {
					back = false;
				};
				if (cy != CHUNK_LENGTH - 1) if (c.chunkBlockData[get3dCoord(cx, cy + 1, cz)] != 32) {
					front = false;
				};
				if (cz != 0) if (c.chunkBlockData[get3dCoord(cx, cy, cz - 1)] != 32) {
					bottom = false;
				};
				if (cz == 0) {
					bottom = false;
				};
				if (cz != CHUNK_HEIGHT - 1) if (c.chunkBlockData[get3dCoord(cx, cy, cz + 1)] != 32) {
					top = false;
				};

				//store 6 floats in 1 float
				//5 bits for x; 5 bits for y; 8 bits for z; 1 bit for TexID.x; 1 bit for TexID.y; 5 bit for texID; 25 bits total, 7 unoccupied
				//max x,y = 31; max z = 255; max t,s = 1; max texID = 32
				//0b 00000 00000 00000000 0 0 00000 -------
				//
				union floatToUli {
					float floatData;
					unsigned long int uliData;
				};
				floatToUli vertexData{};
				unsigned long int container{}; //same bit length as a float

				int faceVertices = 0;
				if (back) {
					for (int j = 0; j < 6; j++) {
						vertexData.uliData = 0;
						container = 0;

						//blockPosition.x
						container = (blockVertices[j * 5] + blockPosition.x);
						container = container << (sizeof(float) * 8 - 5); //first 5
						vertexData.uliData |= container;

						//blockPosition.y
						container = (blockVertices[j * 5 + 1] + blockPosition.y);
						container = container << (sizeof(float) * 8 - 5 - 5); // 6 to 10
						vertexData.uliData |= container;

						//blockPosition.z
						container = (blockVertices[j * 5 + 2] + blockPosition.z);
						container = container << (sizeof(float) * 8 - 5 - 5 - 8); // 11 to 18
						vertexData.uliData |= container;

						//texture coordintae s
						container = (blockVertices[j * 5 + 3]);
						container = container << (sizeof(float) * 8 - 5 - 5 - 8 - 1); //19
						vertexData.uliData |= container;

						//texture coordintae t
						container = (blockVertices[j * 5 + 4]);
						container = container << (sizeof(float) * 8 - 5 - 5 - 8 - 1 - 1); //20
						vertexData.uliData |= container;

						//texture ID
						container = static_cast<unsigned long int>(blockID);
						container = container << (sizeof(float) * 8 - 5 - 5 - 8 - 1 - 1 - 5); //21 to 25
						vertexData.uliData |= container;

						vertices.push_back(vertexData.floatData);

						//vertices.push_back(blockVertices[j * 5] + blockPosition.x);
						//vertices.push_back(blockVertices[j * 5 + 1] + blockPosition.y);
						//vertices.push_back(blockVertices[j * 5 + 2] + blockPosition.z);
						//vertices.push_back(blockVertices[j * 5 + 3]);
						//vertices.push_back(blockVertices[j * 5 + 4]);
						//vertices.push_back(static_cast<float>(blockID));
					}
					faceVertices += 6;
				}
				if (front) {
					for (int j = 6; j < 12; j++) {
						vertexData.uliData = 0;
						container = 0;

						//blockPosition.x
						container = (blockVertices[j * 5] + blockPosition.x);
						container = container << (sizeof(float) * 8 - 5); //first 5
						vertexData.uliData |= container;

						//blockPosition.y
						container = (blockVertices[j * 5 + 1] + blockPosition.y);
						container = container << (sizeof(float) * 8 - 10); // 6 to 10
						vertexData.uliData |= container;

						//blockPosition.z
						container = (blockVertices[j * 5 + 2] + blockPosition.z);
						container = container << (sizeof(float) * 8 - 18); // 11 to 18
						vertexData.uliData |= container;

						//texture coordintae s
						container = (blockVertices[j * 5 + 3]);
						container = container << (sizeof(float) * 8 - 19); //19
						vertexData.uliData |= container;

						//texture coordintae t
						container = (blockVertices[j * 5 + 4]);
						container = container << (sizeof(float) * 8 - 20); //20
						vertexData.uliData |= container;

						//texture ID
						container = static_cast<unsigned long int>(blockID);
						container = container << (sizeof(float) * 8 - 25); //21 to 25
						vertexData.uliData |= container;

						vertices.push_back(vertexData.floatData);
					}
					faceVertices += 6;
				}
				if (left) {
					for (int j = 12; j < 18; j++) {
						vertexData.uliData = 0;
						container = 0;

						//blockPosition.x
						container = (blockVertices[j * 5] + blockPosition.x);
						container = container << (sizeof(float) * 8 - 5); //first 5
						vertexData.uliData |= container;

						//blockPosition.y
						container = (blockVertices[j * 5 + 1] + blockPosition.y);
						container = container << (sizeof(float) * 8 - 10); // 6 to 10
						vertexData.uliData |= container;

						//blockPosition.z
						container = (blockVertices[j * 5 + 2] + blockPosition.z);
						container = container << (sizeof(float) * 8 - 18); // 11 to 18
						vertexData.uliData |= container;

						//texture coordintae s
						container = (blockVertices[j * 5 + 3]);
						container = container << (sizeof(float) * 8 - 19); //19
						vertexData.uliData |= container;

						//texture coordintae t
						container = (blockVertices[j * 5 + 4]);
						container = container << (sizeof(float) * 8 - 20); //20
						vertexData.uliData |= container;

						//texture ID
						container = static_cast<unsigned long int>(blockID);
						container = container << (sizeof(float) * 8 - 25); //21 to 25
						vertexData.uliData |= container;

						vertices.push_back(vertexData.floatData);
					}
					faceVertices += 6;
				}
				if (right) {
					for (int j = 18; j < 24; j++) {
						vertexData.uliData = 0;
						container = 0;

						//blockPosition.x
						container = (blockVertices[j * 5] + blockPosition.x);
						container = container << (sizeof(float) * 8 - 5); //first 5
						vertexData.uliData |= container;

						//blockPosition.y
						container = (blockVertices[j * 5 + 1] + blockPosition.y);
						container = container << (sizeof(float) * 8 - 10); // 6 to 10
						vertexData.uliData |= container;

						//blockPosition.z
						container = (blockVertices[j * 5 + 2] + blockPosition.z);
						container = container << (sizeof(float) * 8 - 18); // 11 to 18
						vertexData.uliData |= container;

						//texture coordintae s
						container = (blockVertices[j * 5 + 3]);
						container = container << (sizeof(float) * 8 - 19); //19
						vertexData.uliData |= container;

						//texture coordintae t
						container = (blockVertices[j * 5 + 4]);
						container = container << (sizeof(float) * 8 - 20); //20
						vertexData.uliData |= container;

						//texture ID
						container = static_cast<unsigned long int>(blockID);
						container = container << (sizeof(float) * 8 - 25); //21 to 25
						vertexData.uliData |= container;

						vertices.push_back(vertexData.floatData);
					}
					faceVertices += 6;
				}
				if (bottom) {
					for (int j = 24; j < 30; j++) {
						vertexData.uliData = 0;
						container = 0;

						//blockPosition.x
						container = (blockVertices[j * 5] + blockPosition.x);
						container = container << (sizeof(float) * 8 - 5); //first 5
						vertexData.uliData |= container;

						//blockPosition.y
						container = (blockVertices[j * 5 + 1] + blockPosition.y);
						container = container << (sizeof(float) * 8 - 10); // 6 to 10
						vertexData.uliData |= container;

						//blockPosition.z
						container = (blockVertices[j * 5 + 2] + blockPosition.z);
						container = container << (sizeof(float) * 8 - 18); // 11 to 18
						vertexData.uliData |= container;

						//texture coordintae s
						container = (blockVertices[j * 5 + 3]);
						container = container << (sizeof(float) * 8 - 19); //19
						vertexData.uliData |= container;

						//texture coordintae t
						container = (blockVertices[j * 5 + 4]);
						container = container << (sizeof(float) * 8 - 20); //20
						vertexData.uliData |= container;

						//texture ID
						container = static_cast<unsigned long int>(blockID);
						container = container << (sizeof(float) * 8 - 25); //21 to 25
						vertexData.uliData |= container;

						vertices.push_back(vertexData.floatData);
					}
					faceVertices += 6;
				}
				if (top) {
					for (int j = 30; j < 36; j++) {
						vertexData.uliData = 0;
						container = 0;

						//blockPosition.x
						container = (blockVertices[j * 5] + blockPosition.x);
						container = container << (sizeof(float) * 8 - 5); //first 5
						vertexData.uliData |= container;

						//blockPosition.y
						container = (blockVertices[j * 5 + 1] + blockPosition.y);
						container = container << (sizeof(float) * 8 - 10); // 6 to 10
						vertexData.uliData |= container;

						//blockPosition.z
						container = (blockVertices[j * 5 + 2] + blockPosition.z);
						container = container << (sizeof(float) * 8 - 18); // 11 to 18
						vertexData.uliData |= container;

						//texture coordintae s
						container = (blockVertices[j * 5 + 3]);
						container = container << (sizeof(float) * 8 - 19); //19
						vertexData.uliData |= container;

						//texture coordintae t
						container = (blockVertices[j * 5 + 4]);
						container = container << (sizeof(float) * 8 - 20); //20
						vertexData.uliData |= container;

						//texture ID
						container = static_cast<unsigned long int>(blockID);
						container = container << (sizeof(float) * 8 - 25); //21 to 25
						vertexData.uliData |= container;

						vertices.push_back(vertexData.floatData);
					}
					faceVertices += 6;
				}

				for (int k = 0; k < faceVertices; k++) {
					indices.push_back(c.indexCount + k);
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

	c.indexCount = indices.size();

	//to MUSI byc w glownym watku zeby OpenGL byl szczesliwy
	glGenVertexArrays(1, &c.VAO);
	glGenBuffers(1, &c.VBO);
	glGenBuffers(1, &c.EBO);

	glBindVertexArray(c.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, c.VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	//atrybut wszystkiego
	glVertexAttribPointer(0, 6, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

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
	newChunk->VBO = 0;

	//set block data
	float perlinNoise[CHUNK_LENGTH * CHUNK_LENGTH]{};
	perlinNoiseGenerator(x, y, perlinNoise, OCTAVES, PERSISTANCE);
	for (uint8_t cx = 0; cx < CHUNK_LENGTH; cx++) {
		for (uint8_t cy = 0; cy < CHUNK_LENGTH; cy++) {
			//uint8_t height = (uint8_t)(((perlinNoise[get2dCoord(cx, cy)] + 1.5f) * 0.36f) * CHUNK_HEIGHT); //TO TRZEBA ZAKTUALIZOWAC DO GENERACJI Z OKTAWAMI
			uint8_t height = (cx % 4 || cy % 4) ? 2 : 90;
			
			for (uint8_t cz = 0; cz <= height - 2; ++cz) {
				newChunk->chunkBlockData[get3dCoord(cx, cy, cz)] = 2; //Stone
			}
			newChunk->chunkBlockData[get3dCoord(cx, cy, height - 1)] = 1; // Dirt
			newChunk->chunkBlockData[get3dCoord(cx, cy, height)] = 0; // Grass
			for (uint8_t cz = height + 1; cz < CHUNK_HEIGHT; ++cz) {
				newChunk->chunkBlockData[get3dCoord(cx, cy, cz)] = 32; //Air
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
		glDeleteBuffers(1, &(it->second->VBO));
		glDeleteBuffers(1, &(it->second->EBO));
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
	return a + weight * (b - a);
}

float world::fade(float a) { //ttv/faide
	return a * a * a * (a * (a * 6 - 15) + 10);
	//dla a (0,1) wartosci (0, 10)
}

std::pair<float, float> world::gradientRNG(int i_x, int i_y) { //-1 do 1
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

	std::pair<float, float> p;
	p.first = sin(random);
	p.second = cos(random);

	return p;
};

float world::scalarProduct(float dx, float dy, std::pair<float, float> gradient) {
	return (dx * gradient.first + dy * gradient.second);
	// -2 do 2
}

void world::perlinNoiseOctave(int chunkX, int chunkY, float* perlinNoise, float frequency, float amplitude) {
	//wspolrzedne krawedzi siatki
	int x0 = chunkX * CHUNK_LENGTH;
	int y0 = chunkY * CHUNK_LENGTH;
	int x1 = x0 + CHUNK_LENGTH;
	int y1 = y0 + CHUNK_LENGTH;

	//krawedzie siatki
	float k00, k10, k11, k01;
	float dwn, up;

	//gradient na krawedziach
	std::pair<float, float> gradient00 = gradientRNG(x0, y0);
	std::pair<float, float> gradient10 = gradientRNG(x1, y0);
	std::pair<float, float> gradient11 = gradientRNG(x1, y1);
	std::pair<float, float> gradient01 = gradientRNG(x0, y1);

	//wagi interpolacji
	float wx, wy;

	for (uint8_t x = 0; x < CHUNK_LENGTH; x++) {
		wx = fade(x * CHUNK_LENGTH_RECIPROCAL);
		for (uint8_t y = 0; y < CHUNK_LENGTH; y++) {
			wy = fade(y * CHUNK_LENGTH_RECIPROCAL);

			//obliczenia krawedzi
			k00 = scalarProduct(x * CHUNK_LENGTH_RECIPROCAL, y * CHUNK_LENGTH_RECIPROCAL, gradient00);
			k10 = scalarProduct((x - CHUNK_LENGTH) * CHUNK_LENGTH_RECIPROCAL, y * CHUNK_LENGTH_RECIPROCAL, gradient10);
			k11 = scalarProduct((x - CHUNK_LENGTH) * CHUNK_LENGTH_RECIPROCAL, (y - CHUNK_LENGTH) * CHUNK_LENGTH_RECIPROCAL, gradient11);
			k01 = scalarProduct(x * CHUNK_LENGTH_RECIPROCAL, (y - CHUNK_LENGTH) * CHUNK_LENGTH_RECIPROCAL , gradient01);

			//interpolacja up dwn
			dwn = lerp(k00, k10, wx);
			up = lerp(k01, k11, wx);
			perlinNoise[get2dCoord(x, y)] += lerp(dwn, up, wy) * amplitude;

			/*
			//obliczenie i interpolacja dwn
			k00 = scalarProduct(1 - wx, 1 - wy, gradient00);
			k10 = scalarProduct(wx - 1, 1 - wy, gradient10);
			dwn = cubicInterpolator(k00, k10, wx);

			//obliczenie i interpolacja up
			k11 = scalarProduct(wx - 1, wy - 1, gradient11);
			k01 = scalarProduct(1 - wx, wy - 1, gradient01);
			up = cubicInterpolator(k11, k01, wx);

			// interpolacja
			perlinNoise[get2dCoord(x, y)] = cubicInterpolator(dwn, up, wy);
			*/
		}
	}
}

void world::perlinNoiseGenerator(int chunkX, int chunkY, float* perlinNoise, unsigned int octaves, float persistence) {
	float amplitude = 1;
	float frequency = 1;

	for (unsigned int octave = 0; octave < octaves; octave++) {
		perlinNoiseOctave(chunkX, chunkY, perlinNoise, frequency, amplitude);
		amplitude *= persistence;
		frequency *= 2;
	}
}