#pragma once
#include <memory>
#include "../GameObject.h"
#include "ComponentID.h"

namespace Eugene
{

	class Component
	{
	public:
		virtual ~Component();
		void SetOwner(const GameObjectWeakPtr& owner);
		virtual ComponentID GetID(void) const = 0;
		virtual void Begine(void) = 0;
		virtual void End(void) = 0;
	protected:
		GameObjectWeakPtr owner_;
	};
}
