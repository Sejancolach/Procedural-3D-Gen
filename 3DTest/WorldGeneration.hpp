#pragma once
#include "Behaviour.hpp"
#include "GL/glew.h"
#include <unordered_map>
#include <utility>
#include <queue>
#include <future>

class Behaviour;


struct pair_hash {
	template <class T1, class T2>
	std::size_t operator () (const std::pair<T1, T2>& p) const {
		return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second);
	}
};

class WorldGeneration : public Behaviour { 
private:
	static GameObject* GenerateChunk(int posX, int posY, int size);
	//std::vector<GameObject*> chunks;
	std::unordered_map<std::pair<int,int>, GameObject*, pair_hash> chunks;
	std::queue<std::future<GameObject*>> newChunks;

	int chunkSize = 64;

public:
	GLuint shaderID;
	GLuint textureID1;
	GLuint textureID2;

	void GenerateWorld(void);

	void Update();
};

