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

namespace Component {
	class Transform;
	class MeshRender;
}
class GameObject;

void WorldGeneration::GenerateWorld(void) { 
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::vector<std::future<GameObject*>> futures;
	
	int size = 16;
	int chunkSize = 64;

	for(int x = -size; x <= size; x++) {
		for(int y = -size; y <= size; y++) {
			futures.push_back(std::move(std::async(std::launch::async, WorldGeneration::GenerateChunk, x, y, chunkSize,*this)));
		}
	}

	for(int i = 0; i < futures.size(); i++) {
		chunks.push_back(futures[i].get());
		Component::MeshRender* mRender = static_cast<Component::MeshRender*>(chunks[i]->GetComponent<Component::MeshRender>());
		mRender->mesh->UpdateBuffers(); // Buffers can only be Updated on the main thread
		mRender->ShaderID = shaderID;
		mRender->TextureID = textureID1;
		mRender->TextureID2 = textureID2;
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << " time to Generate World -> " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "mrs\t" <<
			std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" <<std::endl;
	//TODO: smooth normals between chunk meshes 
}

GameObject* WorldGeneration::GenerateChunk(int posX, int posY, int size, const WorldGeneration& ref) { 
	GameObject* chunk = new GameObject();
	Component::MeshRender* mRender = new Component::MeshRender();
	mRender->ShaderID = ref.shaderID;
	mRender->TextureID = ref.textureID1;
	mRender->TextureID2 = ref.textureID2;

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
	//						  return Mathf::CellularNoise2D((x + posX), (y + posY), 1.0f,seed) * 4;
	//					  },
	//					  false)); // <-- disable buffer updating, can only be done on the main thread
	mesh->SmoothNormals();
	chunk->transform->SetPosition(glm::vec3(posX * size + halfSize, 0, posY * size + halfSize));
	mRender->mesh = mesh;
	chunk->AddComponent(mRender);
	return chunk;
}
