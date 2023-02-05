#pragma once
#include <memory>
#include <deque>
#include <optional>

namespace Eugene
{
	class GameObject
	{
	public:
		using GameObjectUPtr = std::unique_ptr<GameObject>;
		using Children = std::deque<GameObjectUPtr>;
		using GameObjectRef = std::optional<const GameObject*>;
		virtual ~GameObject();

		void AddChild(GameObjectUPtr&& gameObject);
		void RemoveChild(std::uint64_t index);
		void RemoveChild(void);
		GameObjectRef GetChild(std::uint64_t index);
		std::uint64_t GetChildCount(void) const;
	protected:
		GameObject();
		Children children_;
		bool isActive_;
	private:
	};

}