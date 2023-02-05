#pragma once

namespace Eugene
{
#define EugeneEngine (Engine::GetInstance())
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

		/// <summary>
		/// �����_�����O����
		/// </summary>
		/// <param name=""></param>
		void Rendering(void);

		/// <summary>
		/// �Q�[������
		/// </summary>
		/// <param name=""></param>
		void Game(void);
	};
}


extern "C" __declspec(dllexport) int __cdecl  Run(const char* scene);