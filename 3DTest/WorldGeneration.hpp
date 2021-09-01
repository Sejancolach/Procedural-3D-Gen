#pragma once
#include "Behaviour.hpp"
#include "GL/glew.h"
class Behaviour;

class WorldGeneration : public Behaviour { 
private:
	static GameObject* GenerateChunk(int posX, int posY, int size, const WorldGeneration& ref);
	std::vector<GameObject*> chunks;
public:
	GLuint shaderID;
	GLuint textureID1;
	GLuint textureID2;

	void GenerateWorld(void);
};

