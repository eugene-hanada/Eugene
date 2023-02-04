#pragma once

namespace Eugene
{
	class Engine
	{
	public:
		static Engine& GetInstance(void);
		int Run(void);
	private:
		Engine();
		~Engine();
		Engine(const Engine&) = delete;
		void operator=(const Engine&) = delete;

		void Rendering(void);
		void Game(void);
	};
}


extern "C" __declspec(dllexport) int __cdecl  Run(const char* scene);