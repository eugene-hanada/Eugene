#include "../Include/GameObject/Component/Renderer.h"

#pragma comment(lib, "Eugene.lib")



namespace Eugene
{
	class SceneRenderrer :
		public Eugene::Renderer
	{
	public:
		SceneRenderrer()
		{

		}
		~SceneRenderrer() override
		{

		}
	private:
		void SetCommandList(Eugene::CommandList& cmdList) final
		{

		}
		void Begine(void) final
		{

		}

		void End(void) final
		{

		}
	};
}

extern "C" __declspec(dllexport) Eugene::Component * __cdecl  CreateSceneRenderer(void)
{
	return new Eugene::SceneRenderrer{};
}

