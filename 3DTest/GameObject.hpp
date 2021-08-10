#pragma once
#include <vector>
#include "Object.hpp"
#include <typeinfo>
#include <typeindex>
#include <stdio.h>
#include <stdlib.h>
#include <mutex>
#include "Behaviour.hpp"
#include <glm/gtc/matrix_transform.hpp>


namespace Component {
    class Component;
    class Transform;
    class MeshRender;
}
class Behaviour;


class GameObject :
    public Object { 
private:
    std::mutex childMutex;
    std::vector<GameObject*> children;
    GameObject* parent;
    std::vector<Component::Component*> components;
    std::vector<Behaviour*> behaviours;

public:

    bool isStatic;
    bool isActive = true;
    Component::Transform* transform;
    GameObject();
    GameObject(GameObject* parent);
    template<typename T>
    Component::Component* GetComponent(void);
    Component::Component* AddComponent(Component::Component* comp);

    template<typename T>
    Behaviour* GetBehaviour(void);
    Behaviour* AddBehaviour(Behaviour* bev);

    std::string name;

    void AddChild(GameObject * child);
    GameObject* GetChild(int idx);
    std::vector<GameObject*> GetChildren();
    
    GameObject* GetParent(void);
    void SetParent(GameObject* newParent);

    void Update(void);
    void Render(glm::mat4x4 mvp);
    void LateUpdate(void);
    void FixedUpdate(void);

    const glm::mat4x4& getTransformMatrix(void);
};
template<typename T>
inline Component::Component* GameObject::GetComponent() {
    for(auto& comp : components)     {
        if(dynamic_cast<T*>(comp))
            return dynamic_cast<T*>(comp);
    }
    return nullptr;
}

template<typename T>
inline Behaviour* GameObject::GetBehaviour(void) {
    for(auto& bev : behaviours) {
        if(dynamic_cast<T*>(bev))
            return dynamic_cast<T*>(bev);
    }
    return nullptr;
}