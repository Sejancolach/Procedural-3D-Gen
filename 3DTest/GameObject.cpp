#include "GameObject.hpp"
#include "Transform.hpp"
#include <algorithm>
#include <vector>
#include "SceneManager.hpp"
#include "Component.hpp"
#include "MeshRender.hpp"

GameObject::GameObject() {
	transform = new Component::Transform();
	components = {};

	isStatic = false;
	isActive = true;

	SceneManager::getInstance()->AddToCurrentScene(this);
}

GameObject::GameObject(GameObject* parent) { 
	transform = new Component::Transform();
	components = {};
	isStatic = false;
	isActive = true;
	
}

Component::Component* GameObject::AddComponent(Component::Component* comp) {
	components.push_back(comp);
	comp->gameObject = this;
	return comp;
}

Behaviour* GameObject::AddBehaviour(Behaviour* bev) {
	behaviours.push_back(bev);
	bev->gameobject = this;
	return bev;
}

void GameObject::AddChild(GameObject* child) { 
	std::lock_guard<std::mutex> lock(childMutex);
	children.push_back(child);
	child->parent = this;
}

GameObject* GameObject::GetChild(int idx) {
	std::lock_guard<std::mutex> lock(childMutex);
	if(idx > children.size()) return nullptr;
	return children[idx];
}

std::vector<GameObject*> GameObject::GetChildren() {
	std::lock_guard<std::mutex> lock(childMutex);
	return children;
}

GameObject* GameObject::GetParent() {
	return parent;
}

void GameObject::SetParent(GameObject* newParent) { 
	parent = newParent;
}

void GameObject::Update(void) { 
	for(int i = 0; i < behaviours.size(); i++) {
		behaviours[i]->Update();
	}
}

void GameObject::Render(glm::mat4x4 mvp) {
	if(!isActive) return; //if not active don't render object or children
	Component::MeshRender* mRender = static_cast<Component::MeshRender*>(GetComponent<Component::MeshRender>());

	if(mRender != nullptr) {
		mRender->Render(mvp);
	}
	//else if Different Renderer (Bone Mesh Renderer, 2DRenderer)
	for(int i = 0; i < children.size(); i++){
		children[i]->Render(mvp);
	}
}

void GameObject::ShadowRender(glm::mat4 mvp) { 
	if(!isActive) return; //if not active don't render object or children
	Component::MeshRender* mRender = static_cast<Component::MeshRender*>(GetComponent<Component::MeshRender>());

	if(mRender != nullptr) {
		mRender->ShadowRender(mvp);
	}
	//else if Different Renderer (Bone Mesh Renderer, 2DRenderer)
	for(int i = 0; i < children.size(); i++) {
		children[i]->ShadowRender(mvp);
	}
}

void GameObject::LateUpdate(void) { 
	for(int i = 0; i < behaviours.size(); i++) {
		behaviours[i]->LateUpdate();
	}
}

void GameObject::FixedUpdate(void) {
	for(int i = 0; i < behaviours.size(); i++) {
		behaviours[i]->FixedUpdate();
	}
}

//TODO: if Objects are not renderd correctly this might be it
//EDIT: I was in fact right ... future me fix this
const glm::mat4x4& GameObject::getTransformMatrix(void) {
	if(parent != nullptr)
		return parent->getTransformMatrix() * transform->getMatrix();
	return transform->getMatrix();
}

