#pragma once
#include <filesystem>
#include <map>
#include "Resource/Texture.h"

namespace Eugene
{
	class ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager();
		Texture LoadTexture(const std::filesystem::path& path);

	private:
		std::map<std::uint64_t, std::shared_ptr<ImageResource>> textureCache_;
	};
}
