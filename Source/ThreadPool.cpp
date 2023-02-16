#include "../Include/ThreadPool.h"

Eugene::ThreadPool::ThreadPool() :
	cntSmp_{ 0 }
{
	workers_.resize(std::thread::hardware_concurrency() - 3u);
	for (auto&& w : workers_)
	{
		w = std::make_unique<Worker>(*this);
	}
}

Eugene::TaskHandle Eugene::ThreadPool::AddTask(std::function<void(void)>&& task)
{
	auto t = std::packaged_task{ task};
	auto f = t.get_future();

	bsmp_.acquire();
	taskList_.emplace_back(std::move(t));
	bsmp_.release();
	cntSmp_.release();


	return {std::move(f)};
}

void Eugene::ThreadPool::WaitAll(void)
{
	std::deque<std::binary_semaphore> bsmp;
	std::deque<TaskHandle> hList;
	for (auto& w : workers_)
	{
		bsmp.emplace_back(0);
	}

	for (auto& b : bsmp)
	{
		hList.emplace_back(AddTask([&b]() {b.acquire(); }));
	}
	
	bool flag = true;
	while (flag)
	{
		bsmp_.acquire();
		if (taskList_.size() <= 0)
		{
			flag = false;
		}
		bsmp_.release();
	}

	std::for_each(bsmp.begin(), bsmp.end(), [](std::binary_semaphore& b) { b.release(); });

	std::for_each(hList.begin(), hList.end(), [](TaskHandle& h) {h.Wait(); });

}

std::packaged_task<void(void)>&& Eugene::ThreadPool::GetTask(void)
{
	cntSmp_.acquire();
	bsmp_.acquire();
	auto task = std::move(taskList_.front());
	taskList_.pop_front();
	bsmp_.release();

	return std::move(task);
}


Eugene::TaskHandle::~TaskHandle()
{
}

bool Eugene::TaskHandle::IsEnd(void)
{
	return future_.wait_for(std::chrono::seconds{ 0 }) == std::future_status::ready;
}

void Eugene::TaskHandle::Wait(void) const
{
	future_.wait();
}

Eugene::TaskHandle::TaskHandle(TaskHandle&& taskHandle) noexcept
{
	future_ = std::move(taskHandle.future_);
}

Eugene::TaskHandle::TaskHandle(std::future<void>&& future) :
	future_{std::move(future)}
{
}

Eugene::Worker::Worker(ThreadPool& threadPool) :
	threadPool_{threadPool}
{
	isRun_.store(true);
	thread_ = std::thread{ &Worker::Run,this };
}

Eugene::Worker::~Worker()
{
	thread_.join();
}

void Eugene::Worker::End(void)
{
	isRun_.store(false);
}

void Eugene::Worker::Run(void)
{
	while (isRun_.load())
	{
		task_ = std::move(threadPool_.GetTask());
		task_();
	}
}
