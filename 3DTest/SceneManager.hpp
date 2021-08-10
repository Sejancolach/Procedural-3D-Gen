#pragma once
#include "GameObject.hpp"
#include <mutex>
class SceneManager { 

private:
	static std::mutex instance_mutex;
public:
	GameObject *currentScene;

	SceneManager();

	void AddToCurrentScene(GameObject* go);
	//TODO: Object Deletion
	void RemoveFromCurrentScene(GameObject* go);
	void Update(void);
	void Render(glm::mat4x4 mvp);
	void LateUpdate(void);
	void FixedUpdate(void);

	static SceneManager* getInstance();
};

static SceneManager *SceneManagerinstance;