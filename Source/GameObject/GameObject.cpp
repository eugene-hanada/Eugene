#include "../../Include/GameObject/GameObject.h"
#include <Common/Debug.h>
#include <assert.h>

Eugene::GameObject::~GameObject()
{
}

void Eugene::GameObject::AddChild(GameObjectPtr&& gameObject)
{
}

void Eugene::GameObject::RemoveChild(std::uint64_t index)
{
}

void Eugene::GameObject::RemoveChild(void)
{
}

Eugene::GameObjectWeakPtr Eugene::GameObject::GetChild(std::uint64_t index) const
{
	//
	return children_[index];
}

std::uint64_t Eugene::GameObject::GetChildCount(void) const
{
	return children_.size();
}

Eugene::GameObject::GameObject() :
	isActive_{false}
{
}

Eugene::GameObjectPtr::GameObjectPtr(GameObject* ptr)
{
	std::weak_ptr<void*> p;
	p.lock();
}

Eugene::GameObjectPtr::~GameObjectPtr()
{
	delete ptr_;
	ptr_ = nullptr;
}

Eugene::GameObject& Eugene::GameObjectPtr::operator->(void)
{
	return *ptr_;
}




Eugene::GameObjectPtr& Eugene::GameObjectPtr::operator=(const GameObjectPtr& gameObjectPtr)
{
	ptr_ = gameObjectPtr.ptr_ ;
	return *this;
}

Eugene::GameObjectWeakPtr::GameObjectWeakPtr()
{
}

Eugene::GameObjectWeakPtr::GameObjectWeakPtr(const GameObjectPtr& ptr)
{
}

Eugene::GameObject& Eugene::GameObjectWeakPtr::operator->(void)
{
	if (*ptr_ == nullptr)
	{
		DebugLog("QÆæ‚ªíœ‚³‚ê‚Ä‚¢‚Ü‚·");
		assert(false);
	}
	return *ptr_;
}
