#pragma once
#include "Transform.h"
#include "../../../EugeneLib/Include/Math/Vector2.h"

namespace Eugene
{
	class Transform2D :
		public Transform
	{
	public:
		
	private:
		Vector2 pos_;
		Vector2 loacalPos_;
		float rotation_;
	};
}
