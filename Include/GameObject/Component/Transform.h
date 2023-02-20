#pragma once
#include "Component.h"

namespace Eugene
{
	class CommandList;

	class Transform :
		public Component
	{
	public:
		static constexpr ComponentID baseID_{ ComponentID::Transform };
		ComponentID GetID() const final
		{
			return baseID_;
		}
	private:
	};
}
