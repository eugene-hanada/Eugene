#include "../../Include/GameObject/GameObject.h"
#include <Common/Debug.h>
#include <assert.h>
#include "../../Include/GameObject/Component/Component.h"

Eugene::GameObject::~GameObject()
{
}

void Eugene::GameObject::Begin(void)
{
}

void Eugene::GameObject::End(void)
{
}

void Eugene::GameObject::AddChild(GameObjectPtr&& gameObject)
{
	children_.emplace_back(std::move(gameObject));
}

void Eugene::GameObject::RemoveChild(std::uint64_t index)
{
	children_.erase(children_.begin() + index);
}

void Eugene::GameObject::RemoveChild(void)
{
	children_.clear();
}

Eugene::GameObjectWeakPtr Eugene::GameObject::GetChild(std::uint64_t index) const
{
	if (children_.size() <= index)
	{
		DebugLog("{}は範囲外のインデックスです", index);
		index = 0;
		assert(false);
	}
	return children_[index];
}

std::uint64_t Eugene::GameObject::GetChildCount(void) const
{
	return children_.size();
}

bool Eugene::GameObject::AddComponent(std::shared_ptr<Component>&& component)
{
	auto result = components_.try_emplace(component->GetID(), std::move(component));
	return result.second;
}

Eugene::GameObject::GameObject() :
	isActive_{false}
{
}

Eugene::GameObjectPtr::GameObjectPtr(GameObject* ptr)
{
	ptr_.reset(ptr);
}

Eugene::GameObjectPtr::~GameObjectPtr()
{
}

Eugene::GameObject* Eugene::GameObjectPtr::operator->(void)
{
	return ptr_.get();
}

Eugene::GameObjectPtr& Eugene::GameObjectPtr::operator=(const GameObjectPtr& gameObjectPtr)
{
	ptr_ = gameObjectPtr.ptr_ ;
	return *this;
}

Eugene::GameObjectPtr::GameObjectPtr(GameObjectPtr&& gameObjectPtr)
{
	ptr_ = std::move(gameObjectPtr.ptr_);
}

Eugene::GameObjectPtr& Eugene::GameObjectPtr::operator=(GameObjectPtr&& gameObjectPtr)
{
	ptr_ = std::move(gameObjectPtr.ptr_);
	return *this;
}

Eugene::GameObjectWeakPtr::GameObjectWeakPtr()
{
}

Eugene::GameObjectWeakPtr::GameObjectWeakPtr(const GameObjectPtr& ptr)
{
	ptr_ = ptr.ptr_;
}

Eugene::GameObject* Eugene::GameObjectWeakPtr::operator->(void)
{
	if (ptr_.expired())
	{
		DebugLog("参照先が削除されています");
		assert(false);
		return nullptr;
	}
	return ptr_.lock().get();
}
