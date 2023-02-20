#pragma once
#include <memory>
#include <deque>
#include <map>
#include "Component/ComponentID.h"
#include "../Common.h"

namespace Eugene
{
	class GameObject;
	class GameObjectWeakPtr;

	class Component;
	class Transform;
	class Transform2D;

	template<class T>
	concept ComponentBase = requires(T t)
	{
		std::is_same<T, Component>::value();
		T::baseID_;
		std::is_same<decltype(T::baseID_), ComponentID>::value();
	};


	class GameObjectPtr
	{
	public:
		GameObjectPtr(GameObject* ptr);
		~GameObjectPtr();
		GameObject& operator->(void);
		GameObjectWeakPtr operator=(GameObjectPtr& gameObjectPtr);
		GameObjectPtr(GameObjectPtr&& gameObjectPtr);
		GameObjectPtr& operator=(GameObjectPtr&& gameObjectPtr);
	private:
		GameObjectPtr(const GameObjectPtr&) = delete;
		GameObjectPtr& operator=(const GameObjectPtr& gameObjectPtr);
		std::shared_ptr<GameObject> ptr_;
		friend class GameObjectWeakPtr;
	};

	class GameObjectWeakPtr
	{
	public:
		GameObjectWeakPtr();
		GameObjectWeakPtr(const GameObjectPtr& ptr);
		GameObject& operator->(void);
	private:
		std::weak_ptr<GameObject> ptr_;
	};

	class GameObject
	{
	public:
		using Children = std::deque<GameObjectPtr>;
		using ComponentMap = std::map<ComponentID, std::shared_ptr<Component>>;
		virtual ~GameObject();

		void AddChild(GameObjectPtr&& gameObject);
		void RemoveChild(std::uint64_t index);
		void RemoveChild(void);
		GameObjectWeakPtr GetChild(std::uint64_t index) const;
		std::uint64_t GetChildCount(void) const;

		bool AddComponent(std::shared_ptr<Component>&& component);

		template<ComponentBase T>
		void RemoveComponent(void)
		{
			components_.erase(T::baseID_);
		}

		template<ComponentBase T>
		std::weak_ptr<T> GetComponent(void) const
		{
			
			if (!components_.contains(T::baseID_))
			{
				// ‚È‚¢Žž‹ó‚Ìweak_ptr‚ð•Ô‚·
				return std::weak_ptr<T>();
			}

			if constexpr (std::is_same<Transform,T>::value)
			{
				return std::static_pointer_cast<T>(components_[T::baseID_]);
			}

			return std::dynamic_pointer_cast<T>(components_[T::baseID_]);
		}


	protected:
		GameObject();
		Children children_;
		ComponentMap components_;
		bool isActive_;
	private:
	};

	
}