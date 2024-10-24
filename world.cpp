#include "world.h"
#include <iostream>
#include <chrono>
#include <thread>

world::world(const player& playerOne, camera& cam) : playerOne(playerOne), cam(cam) {}

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
	//auto t1 = std::chrono::high_resolution_clock::now();
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

				if (blockType != 255) {
					blockAABB.min.x = x;
					blockAABB.min.y = y;
					blockAABB.min.z = z;
					blockAABB.max.x = x + 1.0f;
					blockAABB.max.y = y + 1.0f;
					blockAABB.max.z = z + 1.0f;

					//coordinates method
					/*
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
					*/

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

	//auto t2 = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	//std::cout << duration << std::endl;
}

void world::createChunks() {
	auto t1 = std::chrono::high_resolution_clock::now();
	playerPosition = playerOne.getPlayerPosition();
	playerChunkX = playerPosition.x * CHUNK_LENGTH_RECIPROCAL;
	playerChunkY = playerPosition.y * CHUNK_LENGTH_RECIPROCAL;
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
				if (blockID == 255) continue; //skip air
				blockPosition = glm::vec3(cx, cz, cy);

				//check neighbours
				if (cx != 0) if (c.chunkBlockData[get3dCoord(cx - 1, cy, cz)] != 255) {
					left = false;
				};
				if (cx != CHUNK_LENGTH - 1) if (c.chunkBlockData[get3dCoord(cx + 1, cy, cz)] != 255) {
					right = false;
				};
				if (cy != 0) if (c.chunkBlockData[get3dCoord(cx, cy - 1, cz)] != 255) {
					back = false;
				};
				if (cy != CHUNK_LENGTH - 1) if (c.chunkBlockData[get3dCoord(cx, cy + 1, cz)] != 255) {
					front = false;
				};
				if (cz != 0) if (c.chunkBlockData[get3dCoord(cx, cy, cz - 1)] != 255) {
					bottom = false;
				};
				if (cz == 0) {
					bottom = false;
				};
				if (cz != CHUNK_HEIGHT - 1) if (c.chunkBlockData[get3dCoord(cx, cy, cz + 1)] != 255) {
					top = false;
				};

				int faceVertices = 0;
				if (back) {
					for (int j = 0; j < 6; j++) {
						vertices.push_back(blockVertices[j * 5] + blockPosition.x);
						vertices.push_back(blockVertices[j * 5 + 1] + blockPosition.y);
						vertices.push_back(blockVertices[j * 5 + 2] + blockPosition.z);
						vertices.push_back(blockVertices[j * 5 + 3]);
						vertices.push_back(blockVertices[j * 5 + 4]);
						vertices.push_back((float)blockID);
					}
					faceVertices += 6;
				}
				if (front) {
					for (int j = 6; j < 12; j++) {
						vertices.push_back(blockVertices[j * 5] + blockPosition.x);
						vertices.push_back(blockVertices[j * 5 + 1] + blockPosition.y);
						vertices.push_back(blockVertices[j * 5 + 2] + blockPosition.z);
						vertices.push_back(blockVertices[j * 5 + 3]);
						vertices.push_back(blockVertices[j * 5 + 4]);
						vertices.push_back((float)blockID);
					}
					faceVertices += 6;
				}
				if (left) {
					for (int j = 12; j < 18; j++) {
						vertices.push_back(blockVertices[j * 5] + blockPosition.x);
						vertices.push_back(blockVertices[j * 5 + 1] + blockPosition.y);
						vertices.push_back(blockVertices[j * 5 + 2] + blockPosition.z);
						vertices.push_back(blockVertices[j * 5 + 3]);
						vertices.push_back(blockVertices[j * 5 + 4]);
						vertices.push_back((float)blockID);
					}
					faceVertices += 6;
				}
				if (right) {
					for (int j = 18; j < 24; j++) {
						vertices.push_back(blockVertices[j * 5] + blockPosition.x);
						vertices.push_back(blockVertices[j * 5 + 1] + blockPosition.y);
						vertices.push_back(blockVertices[j * 5 + 2] + blockPosition.z);
						vertices.push_back(blockVertices[j * 5 + 3]);
						vertices.push_back(blockVertices[j * 5 + 4]);
						vertices.push_back((float)blockID);
					}
					faceVertices += 6;
				}
				if (bottom) {
					for (int j = 24; j < 30; j++) {
						vertices.push_back(blockVertices[j * 5] + blockPosition.x);
						vertices.push_back(blockVertices[j * 5 + 1] + blockPosition.y);
						vertices.push_back(blockVertices[j * 5 + 2] + blockPosition.z);
						vertices.push_back(blockVertices[j * 5 + 3]);
						vertices.push_back(blockVertices[j * 5 + 4]);
						vertices.push_back((float)blockID);
					}
					faceVertices += 6;
				}
				if (top) {
					for (int j = 30; j < 36; j++) {
						vertices.push_back(blockVertices[j * 5] + blockPosition.x);
						vertices.push_back(blockVertices[j * 5 + 1] + blockPosition.y);
						vertices.push_back(blockVertices[j * 5 + 2] + blockPosition.z);
						vertices.push_back(blockVertices[j * 5 + 3]);
						vertices.push_back(blockVertices[j * 5 + 4]);
						vertices.push_back((float)blockID);
					}
					faceVertices += 6;
				}

				for (int k = 0; k < faceVertices; k++) {
					indices.push_back(c.indexCount + k);
				}

				c.indexCount += faceVertices;

				back = true;
				front = true;
				left = true;
				right = true;
				bottom = true;
				top = true;
			}
		}
	}

	//to MUSI byc w glownym watku zeby OpenGL byl szczesliwy
	glGenVertexArrays(1, &c.VAO);
	glGenBuffers(1, &c.VBO);
	glGenBuffers(1, &c.EBO);

	glBindVertexArray(c.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, c.VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	//atrybut pozycji
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//wspolrzedne tekstury
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//ID tekstury
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

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
				newChunk->chunkBlockData[get3dCoord(cx, cy, cz)] = 255; //Air
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
	const unsigned int widthUI = 8 * sizeof(unsigned int);
	const unsigned int halfwidthUI = widthUI * 0.5;
	unsigned int a = SEED ^ i_x, b = SEED ^ i_y;

	//szahermaher z bitami na podstawie seed
	a *= 3284157443;
	b ^= (a << halfwidthUI) | (a >> (widthUI - halfwidthUI));
	b *= 1911520717;
	a ^= (b << halfwidthUI) | (b >> (widthUI - halfwidthUI));
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

	for (int octave = 0; octave < octaves; octave++) {
		perlinNoiseOctave(chunkX, chunkY, perlinNoise, frequency, amplitude);
		amplitude *= persistence;
		frequency *= 2;
	}
}