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

		void RemoveComponent();

		template<class T>
		std::weak_ptr<T> GetComponent(void) const
		{
			static_assert(std::is_base_of<Component, T>::value);
			if constexpr (std::is_base_of<Transform,T>::value)
			{
				if (components_.contains(ComponentID::Transform))
				{
					if constexpr (std::is_same<T, Transform>::value)
					{
						return std::static_pointer_cast<T>(components_[ComponentID::Transform]);
					}
					else
					{
						return std::dynamic_pointer_cast<T>(components_[ComponentID::Transform]);
					}
				}

			}
			return std::weak_ptr<T>();
		}


	protected:
		GameObject();
		Children children_;
		ComponentMap components_;
		bool isActive_;
	private:
	};

	
}