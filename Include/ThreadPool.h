#pragma once
#include <memory>
#include <thread>
#include <atomic>
#include <semaphore>
#include <future>
#include <functional>
#include <deque>
#include <vector>

namespace Eugene
{
	/// <summary>
	/// タスクハンドル(追加したタスクの終了待機などができる)
	/// </summary>
	class TaskHandle
	{
	public:
		~TaskHandle();

		/// <summary>
		/// タスクが終了しているか?
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		bool IsEnd(void) const;

		/// <summary>
		/// タスク終了まで待機する
		/// </summary>
		/// <param name=""></param>
		void Wait(void) const;
		TaskHandle(TaskHandle&& taskHandle) noexcept;
	private:
		TaskHandle(std::future<void>&& future);

		/// <summary>
		/// フューチャー
		/// </summary>
		std::future<void> future_;

		friend class ThreadPool;
	};

	class Worker;

	/// <summary>
	/// スレッドプールクラス
	/// </summary>
	class ThreadPool
	{
	public:
		ThreadPool();
		~ThreadPool();
		TaskHandle AddTask(std::function<void(void)>&& task);
		void WaitAll(void);
	private:

		/// <summary>
		/// タスクの取得をする
		/// </summary>
		/// <param name=""></param>
		/// <returns> タスクパッケージ </returns>
		std::packaged_task<void(void)> GetTask(void);

		/// <summary>
		/// バイナリセマフォ
		/// </summary>
		std::binary_semaphore bsmp_{ 1 };

		/// <summary>
		/// タスクの数のセマフォ
		/// </summary>
		std::counting_semaphore<> taskCountSmp;

		/// <summary>
		/// ワーカースレッドクラス
		/// </summary>
		std::vector<std::unique_ptr<Worker>> workers_;

		/// <summary>
		/// タスクリスト
		/// </summary>
		std::deque<std::packaged_task<void(void)>> taskList_;

		friend class Worker;
	};

	/// <summary>
	/// 作業スレッドを持つクラス
	/// </summary>
	class Worker
	{
	public:
		Worker(ThreadPool& threadPool);
		~Worker();

		/// <summary>
		/// 実行フラグをfalseにして終了できる状態にする
		/// </summary>
		/// <param name=""></param>
		void End(void);
	private:

		/// <summary>
		/// 内部でスレッドで使用する関数
		/// </summary>
		/// <param name=""></param>
		void Run(void);

		/// <summary>
		/// スレッドプールクラスの参照
		/// </summary>
		ThreadPool& threadPool_;

		/// <summary>
		/// スレッド
		/// </summary>
		std::thread thread_;

		/// <summary>
		/// 実行するタスク
		/// </summary>
		std::packaged_task<void(void)> task_;

		/// <summary>
		/// 実行フラグ
		/// </summary>
		std::atomic_bool isRun_;
	};

}
