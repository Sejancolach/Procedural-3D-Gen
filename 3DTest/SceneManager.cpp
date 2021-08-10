#include "SceneManager.hpp"

SceneManager::SceneManager() { 
    SceneManagerinstance = this;
    currentScene = new GameObject(nullptr);
}

void SceneManager::AddToCurrentScene(GameObject* go) { 
    currentScene->AddChild(go);
}

void SceneManager::Update(void) { 
    currentScene->Update();
}

void SceneManager::Render(glm::mat4x4 mvp) {
    currentScene->Render(mvp);
}

void SceneManager::LateUpdate(void) {
    currentScene->LateUpdate();
}

void SceneManager::FixedUpdate(void) { 
    currentScene->FixedUpdate();
}

SceneManager* SceneManager::getInstance() {
    return SceneManagerinstance;
}
