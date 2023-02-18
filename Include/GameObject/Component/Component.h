#pragma once
#include <memory>
#include "../GameObject.h"

namespace Eugene
{
	class Component
	{
	public:
		virtual ~Component() = default;
		void SetOwner(GameObject::GameObjectRef owner);
	protected:
		GameObject::GameObjectRef owner_;
	};
}
