#include "../../../Include/GameObject/Component/Component.h"
#include <Common/Debug.h>



void Eugene::Component::SetOwner(GameObject::GameObjectRef owner)
{
	if (!owner.has_value())
	{
		DebugLog("owner‚Ì’l‚ª‚ ‚è‚Ü‚¹‚ñ");
	}
	owner_ = owner;
}
