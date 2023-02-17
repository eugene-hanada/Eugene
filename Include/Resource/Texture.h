#pragma once
#include <memory>
#include "../ThreadPool.h"

namespace Eugene
{
	class ImageResource;
	class Texture
	{
	public:
		Texture(std::shared_ptr<ImageResource>& resource, TaskHandle&& handle);
		TaskHandle& GetTaskHandle(void);
	private:
		std::shared_ptr<ImageResource> imgResource_;
		TaskHandle taskHandle_;
	};
}
