#pragma once
#include "GameObject.hpp"

class GameObject;
namespace Component {
	class Component {
	public:
		bool isActive = true;
		GameObject* gameObject = nullptr;
	
		virtual ~Component();
	};
}