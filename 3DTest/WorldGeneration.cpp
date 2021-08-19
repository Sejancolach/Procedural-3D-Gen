#include "WorldGeneration.h"
#include "Mathf.hpp"
#include "MeshRender.hpp"
#include "Transform.hpp"
#include <future>
#include <thread>
#include <algorithm>
#include <vector>

namespace Component {
	class Transform;
	class MeshRender;
}
class GameObject;

void WorldGeneration::GenerateWorld(void) { 
	std::vector<std::future<GameObject*>> futures;
	
	int size = 16;
	int chunkSize = 8;

	for(int x = -size; x < size; x++) {
		for(int y = -size; y < size; y++) {
			futures.push_back(std::move(std::async(std::launch::async, WorldGeneration::GenerateChunk, x, y, chunkSize,*this)));
		}
	}

	for(int i = 0; i < futures.size(); i++) {
		//futures[i].wait();
		chunks.push_back(futures[i].get());
		Component::MeshRender* mRender = static_cast<Component::MeshRender*>(chunks[i]->GetComponent<Component::MeshRender>());
		mRender->mesh->UpdateBuffers(); // Buffers can only be Updated on the main thread
		mRender->ShaderID = shaderID;
		mRender->TextureID = textureID1;
		mRender->TextureID2 = textureID2;
	}
	//TODO: smooth normals between chunk meshes 
}

GameObject* WorldGeneration::GenerateChunk(int posX, int posY, int size, const WorldGeneration& ref) { 
	GameObject* chunk = new GameObject();
	Component::MeshRender* mRender = new Component::MeshRender();
	mRender->ShaderID = ref.shaderID;
	mRender->TextureID = ref.textureID1;
	mRender->TextureID2 = ref.textureID2;

	float nSize = 64;
	float halfSize = size / nSize;
	int detailLevel = 8;
	uint16_t octaves = 16;
	uint32_t seed = 0x1754;
	float lacunarity = 1.3754f;
	float persistence = .7f;
	float multiplier = 128.0f;
	Mesh* mesh = new Mesh(Mesh::CreateFromAlgorithm(size, nSize, detailLevel,
						  [&](float x, float y) -> float {
							  return Mathf::SmoothOctaveNoise2D(x + posX * halfSize, y + posY * halfSize, seed, octaves, lacunarity, persistence) * multiplier;
						  },
						  false)); // <-- disable buffer updating, can only be done on the main thread
	mesh->SmoothNormals();
	chunk->transform->SetPosition(glm::vec3(posX * size, 0, posY * size));
	mRender->mesh = mesh;
	chunk->AddComponent(mRender);
	return chunk;
}
