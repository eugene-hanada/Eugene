#include "../../Include/Resource/Texture.h"


Eugene::Texture::Texture(std::shared_ptr<ImageResource>& resource, TaskHandle&& handle) :
	taskHandle_{std::move(handle)}, imgResource_{resource}
{
}

Eugene::TaskHandle& Eugene::Texture::GetTaskHandle(void)
{
	return taskHandle_;
}
