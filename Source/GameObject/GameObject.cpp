#include "../../Include/GameObject/GameObject.h"

Eugene::GameObject::~GameObject()
{
}

void Eugene::GameObject::AddChild(GameObject::GameObjectUPtr&& gameObject)
{
}

void Eugene::GameObject::RemoveChild(std::uint64_t index)
{
}

void Eugene::GameObject::RemoveChild(void)
{
}

Eugene::GameObject::GameObjectRef Eugene::GameObject::GetChild(std::uint64_t index)
{
	GameObjectRef objRef;
	if (index >= children_.size())
	{
		return objRef;
	}
	objRef = children_[index].get();
	return  objRef;
}

std::uint64_t Eugene::GameObject::GetChildCount(void) const
{
	return children_.size();
}

Eugene::GameObject::GameObject()
{
}
