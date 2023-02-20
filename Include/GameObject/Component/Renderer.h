#pragma once
#include "Component.h"

namespace Eugene
{
	class CommandList;

	class Renderer :
		public Component
	{
	public:
		static constexpr ComponentID baseID_{ ComponentID::Renderer };
		ComponentID GetID(void) const final
		{
			return baseID_;
		}
		virtual void SetCommandList(CommandList& cmdList) = 0;
	private:
	};

}