#include "WorldGeneration.hpp"
#include "Mathf.hpp"
#include "MeshRender.hpp"
#include "Transform.hpp"
#include <future>
#include <thread>
#include <algorithm>
#include <vector>
#include <chrono>
#include <iostream>
#include "Timer.hpp"
#include "Camera.h"

namespace Component {
	class Transform;
	class MeshRender;
}
class GameObject;

void WorldGeneration::GenerateWorld(void) { 
	auto _scpT = Debug::ScopedTimer("World Generator", Debug::kMilli);

	std::vector<std::future<GameObject*>> futures;
	int size = 3;
	const int halfSize = chunkSize * .5f;
	for(int x = -size; x <= size; x++) {
		for(int y = -size; y <= size; y++) {
			futures.push_back(std::move(std::async(std::launch::async, WorldGeneration::GenerateChunk, x, y, chunkSize)));
		}
	}

	for(int i = 0; i < futures.size(); i++) {
		GameObject* currentFuture = futures[i].get();
		chunks[std::pair<int,int>(currentFuture->transform->m_pos.x, currentFuture->transform->m_pos.z)] = currentFuture;
		currentFuture->transform->SetPosition(glm::vec3(currentFuture->transform->m_pos.x * chunkSize + halfSize,0, currentFuture->transform->m_pos.z * chunkSize + halfSize));
		Component::MeshRender* mRender = static_cast<Component::MeshRender*>(currentFuture->GetComponent<Component::MeshRender>());
		mRender->mesh->UpdateBuffers(); // Buffers can only be Updated on the main thread
		mRender->ShaderID = shaderID;
		mRender->TextureID = textureID1;
		mRender->TextureID2 = textureID2;
		currentFuture->isActive = true;
	}
	//TODO: smooth normals between chunk meshes 
}

void WorldGeneration::Update() { 
	//Take Camera Position and check if new chunks need to be generated
	int halfSize = chunkSize * .5f;
	int renderDistance = 8;
	Camera* cam =  Camera::main;
	glm::vec3 camPos = cam->gameobject->transform->getPosition();
	glm::vec3 chunkPos = glm::floor(camPos / (float)chunkSize);

	for(int x = -renderDistance; x <= renderDistance; x++) {
		for(int z = -renderDistance; z <= renderDistance; z++) {
			if(!chunks.contains(std::pair<int, int>(chunkPos.x + x, chunkPos.z + z))) {
				int nx = chunkPos.x + x;
				int nz = chunkPos.z + z;
				newChunks.push(std::move(std::async(std::launch::async, WorldGeneration::GenerateChunk, nx, nz, chunkSize)));
				newChunks.back().wait_for(std::chrono::seconds(0));
				chunks[std::pair<int, int>(nx, nz)] = nullptr;
			}
		}
	}
	for(int i = 0; i < newChunks.size(); i++) {
		if(!newChunks.front()._Is_ready()) break;
		GameObject* currentFuture = newChunks.front().get();
		chunks[std::pair<int, int>(currentFuture->transform->m_pos.x, currentFuture->transform->m_pos.z)] = currentFuture;
		currentFuture->transform->SetPosition(glm::vec3(currentFuture->transform->m_pos.x * chunkSize + halfSize, 0, currentFuture->transform->m_pos.z * chunkSize + halfSize));
		Component::MeshRender* mRender = static_cast<Component::MeshRender*>(currentFuture->GetComponent<Component::MeshRender>());
		mRender->mesh->UpdateBuffers(); // Buffers can only be Updated on the main thread
		mRender->ShaderID = shaderID;
		mRender->TextureID = textureID1;
		mRender->TextureID2 = textureID2;
		currentFuture->isActive = true;
		newChunks.pop();
	}
}

GameObject* WorldGeneration::GenerateChunk(int posX, int posY, int size) { 
	GameObject* chunk = new GameObject();
	chunk->isActive = false;
	Component::MeshRender* mRender = new Component::MeshRender();
	float nSize = 256;
	float halfSize = size / nSize;
	int detailLevel = 2;
	uint16_t octaves = 32;
	uint32_t seed = 0x154;
	float lacunarity = 1.214f;
	//float lacunarity = 1.354f;
	//float lacunarity = 1.3754f;
	float persistence = .855f;
	float multiplier = 512.0f;
	Mesh* mesh = new Mesh(Mesh::CreateFromAlgorithm(size, nSize, detailLevel,
						  [&](float x, float y) -> float {
							  return Mathf::SmoothOctaveNoise2D(x + posX * halfSize, y + posY * halfSize, seed, octaves, lacunarity, persistence) * multiplier;
						  },
						  false)); // <-- disable buffer updating, can only be done on the main thread
	//Mesh* mesh = new Mesh(Mesh::CreateFromAlgorithm(size, nSize, detailLevel,
	//					  [&](float x, float y) -> float {
	//						  return Mathf::CellularNoise2D((x + posX * halfSize), (y + posY * halfSize), 1.0f,seed) * 4;
	//					  },
	//					  false)); // <-- disable buffer updating, can only be done on the main thread
	mesh->SmoothNormals(10,.125f);
	//chunk->transform->SetPosition(glm::vec3(posX * size + halfSize, 0, posY * size + halfSize));
	chunk->transform->SetPosition(glm::vec3(posX, 0, posY));
	mRender->mesh = mesh;
	//mRender->DrawBBDMesh = true;
	chunk->AddComponent(mRender);
	return chunk;
}

