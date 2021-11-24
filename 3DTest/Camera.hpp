#pragma once
#include "Behaviour.hpp"

class Camera : public Behaviour {

public:
	float FOV;

	static Camera* main;

	Camera(float fov = 60.0f);

	glm::mat4x4 GetViewMatrix(void);
};

