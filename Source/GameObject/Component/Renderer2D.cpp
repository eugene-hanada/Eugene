#include "../../../Include/GameObject/Component/Renderer2D.h"
#include "../../../Include/GameObject/GameObject.h"
#include "../../../Include/GameObject/Component/Transform2D.h"

void Eugene::Renderer2D::Begine(void)
{
	transform_ = owner_->GetComponent<Transform2D>();
}
