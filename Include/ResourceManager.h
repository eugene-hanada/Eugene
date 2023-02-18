#pragma once
#include <thread>
#include <semaphore>
#include <filesystem>
#include <functional>
#include <map>

namespace Eugene
{
	class CommandList;
	class GpuEngine;
	class ImageResource;
	class Image;

	class ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager();
		
		std::shared_ptr<ImageResource> LoadTexture(const std::filesystem::path& path);

	private:
		std::map<std::uint64_t, std::shared_ptr<ImageResource>> textureCache_;
		std::list<std::function<void(CommandList&)>> uploadFuncList_;
	};
}
