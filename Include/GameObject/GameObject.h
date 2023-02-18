#pragma once
#include <memory>
#include <deque>
#include <optional>

namespace Eugene
{
	class GameObject;
	class GameObjectWeakPtr;

	class GameObjectPtr
	{
	public:
		GameObjectPtr(GameObject* ptr);
		~GameObjectPtr();
		GameObject& operator->(void);
		GameObjectWeakPtr operator=(GameObjectPtr& gameObjectPtr);
		GameObjectPtr(GameObjectPtr&& gameObjectPtr);
	private:
		GameObjectPtr(const GameObjectPtr& gameObjectPtr) = delete;
		GameObjectPtr& operator=(const GameObjectPtr& gameObjectPtr);
		GameObject* ptr_;
	};

	class GameObjectWeakPtr
	{
	public:
		GameObjectWeakPtr();
		GameObjectWeakPtr(const GameObjectPtr& ptr);
		GameObject& operator->(void);
	private:
		GameObject** ptr_;
	};



	class GameObject
	{
	public:
		using Children = std::deque<GameObjectPtr>;
		using GameObjectRef = std::optional<const GameObject*>;
		virtual ~GameObject();

		void AddChild(GameObjectPtr&& gameObject);
		void RemoveChild(std::uint64_t index);
		void RemoveChild(void);
		GameObjectWeakPtr GetChild(std::uint64_t index) const;
		std::uint64_t GetChildCount(void) const;
	protected:
		GameObject();
		Children children_;

		bool isActive_;
	private:
	};

	
}