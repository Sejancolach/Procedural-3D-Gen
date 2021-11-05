#include "Camera.h"
#include "GameObject.hpp"
#include "Transform.hpp"

Camera::Camera(float fov) { 
	if(Camera::main == nullptr)
		Camera::main = this;
	FOV = fov;
}

glm::mat4x4 Camera::GetViewMatrix(void) {
	glm::vec3 dir = glm::eulerAngles(gameobject->transform->m_rot) * 3.14159f / 180.0f;
	return glm::lookAt(gameobject->transform->m_pos,
					   gameobject->transform->m_pos + dir,
					   glm::vec3(0, 1, 0));
}
Camera* Camera::main = nullptr;