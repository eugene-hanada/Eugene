#include "../../../Include/GameObject/Component/Component.h"
#include <Common/Debug.h>



Eugene::Component::~Component()
{
}

void Eugene::Component::SetOwner(const GameObjectWeakPtr& owner)
{
	owner_ = owner;
}
