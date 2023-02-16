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
	class TaskHandle
	{
	public:
		~TaskHandle();
		bool IsEnd(void);
		void Wait(void) const;
		TaskHandle(TaskHandle&& taskHandle) noexcept;
	private:
		TaskHandle(std::future<void>&& future);
		std::future<void> future_;
		friend class ThreadPool;
	};

	class Worker;

	class ThreadPool
	{
	public:
		ThreadPool();
		TaskHandle AddTask(std::function<void(void)>&& task);
		void WaitAll(void);
	private:

		std::packaged_task<void(void)>&& GetTask(void);
		std::binary_semaphore bsmp_{ 1 };
		std::counting_semaphore<> cntSmp_;
		std::vector<std::unique_ptr<Worker>> workers_;
		std::deque<std::packaged_task<void(void)>> taskList_;
		friend class Worker;
	};

	class Worker
	{
	public:
		Worker(ThreadPool& threadPool);
		~Worker();
		void End(void);
	private:
		void Run(void);
		ThreadPool& threadPool_;
		std::thread thread_;
		std::packaged_task<void(void)> task_;
		std::atomic_bool isRun_;
	};

}
