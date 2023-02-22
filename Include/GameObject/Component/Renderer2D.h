#pragma once
#include "Renderer.h"

namespace Eugene
{
	class Transform2D;

	class Renderer2D :
		public Renderer
	{
	public:
	private:
		void Begine(void) final;

		std::weak_ptr<Transform2D> transform_;
	};
}
