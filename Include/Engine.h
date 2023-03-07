#pragma once
#include <memory>
#include "Engine.h"
#include "GameObject/GameObject.h"
#include "GameObject/Component/Component.h"
#include "ThreadPool.h"

namespace Eugene
{
#define EugeneEngine (Engine::GetInstance())

	class ThreadPool;

	class EUGENE_API Engine
	{
	public:
		static Engine& GetInstance(void);
		int Run(void);
		ThreadPool& GetThreadPool(void)&;
	private:
		Engine();
		~Engine();
		Engine(const Engine&) = delete;
		void operator=(const Engine&) = delete;

		/// <summary>
		/// レンダリング処理
		/// </summary>
		/// <param name=""></param>
		void Rendering(void);

		/// <summary>
		/// ゲーム処理
		/// </summary>
		/// <param name=""></param>
		void Game(void);

		/// <summary>
		/// スレッドプール
		/// </summary>
		std::unique_ptr<ThreadPool> threadPool_;
	};
}


extern "C" EUGENE_API int __cdecl  Run(const char* scene);