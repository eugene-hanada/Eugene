#include "../../../Include/GameObject/Component/Component.h"
#include <Common/Debug.h>



void Eugene::Component::SetOwner(GameObject::GameObjectRef owner)
{
	if (!owner.has_value())
	{
		DebugLog("owner�̒l������܂���");
	}
	owner_ = owner;
}
