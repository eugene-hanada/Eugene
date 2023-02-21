#include "../Include/ThreadPool.h"
#include "../EugeneLib/Include/Common/Debug.h"

Eugene::ThreadPool::ThreadPool() :
	taskCountSmp{ 0 }
{
	// ゲームスレッド、レンダリングスレッド、コピー用スレッド
	// 3つを差し引いた分作成
	workers_.resize(std::max(std::thread::hardware_concurrency() - 3u, 1u));
	for (auto& w : workers_)
	{
		w = std::make_unique<Worker>(*this);
	}
}

Eugene::ThreadPool::~ThreadPool()
{
	// すべてのワーカーを終了にする
	std::for_each(workers_.begin(), workers_.end(), [](std::unique_ptr<Worker>& w) {w->End(); });

	bsmp_.acquire();

	// 未実行のタスクを破棄
	taskList_.clear();
	bsmp_.release();
	
	// 終了を待機する
	WaitAll();
}

Eugene::TaskHandle Eugene::ThreadPool::AddTask(std::function<void(void)>&& task)
{
	// packaged_taskを作成
	auto t = std::packaged_task{ task};

	// future取得
	auto f = t.get_future();

	bsmp_.acquire();

	// タスクを追加
	taskList_.emplace_back(std::move(t));
	bsmp_.release();

	// タスクカウントを加算
	taskCountSmp.release();

	// ハンドルを返す
	return {std::move(f)};
}

void Eugene::ThreadPool::WaitAll(void)
{
	// 待機用セマフォ
	std::deque<std::binary_semaphore> bsmp;

	// ハンドル
	std::deque<TaskHandle> hList;

	// 0でセマフォを作る
	for (auto& w : workers_)
	{
		bsmp.emplace_back(1);
	}

	// 待機のタスクを入れる
	for (auto& b : bsmp)
	{
		AddTask([&b]() {b.acquire(); });
		//hList.emplace_back();
	}
	
	// 継続フラグ
	bool flag = true;

	while (flag)
	{
		// アクセス
		bsmp_.acquire();
		if (taskList_.size() <= 0)
		{
			// タスクリストが空なら継続フラグをfalseに
			flag = false;
		}
		bsmp_.release();

		// 他スレッドに実行の機会を与える
		std::this_thread::yield();
	}

	// 待機させていたタスクを実行する
	std::for_each(bsmp.begin(), bsmp.end(), [](std::binary_semaphore& b) { b.release(); });

	// すべてのタスクを終了するまで待機する
	std::for_each(hList.begin(), hList.end(), [](TaskHandle& h) {h.Wait(); });

}

std::packaged_task<void(void)> Eugene::ThreadPool::GetTask(void)
{
	// タスクリストの数が増えるまで待機
	DebugLog("タスク取得待機");
	taskCountSmp.acquire();

	// タスク取得の制御
	bsmp_.acquire();
	DebugLog("タスク取得");

	// タスクリストの先頭から取得
	auto task = std::move(taskList_.front());
	taskList_.pop_front();
	bsmp_.release();

	// タスクを返す
	return std::move(task);
}


Eugene::TaskHandle::~TaskHandle()
{
}

bool Eugene::TaskHandle::IsEnd(void) const
{
	// ステートがreadyになるまで待機
	return future_.wait_for(std::chrono::seconds{ 0 }) == std::future_status::ready;
}

void Eugene::TaskHandle::Wait(void) const
{
	// 待機する
	future_.wait();
}

Eugene::TaskHandle::TaskHandle(TaskHandle&& taskHandle) noexcept
{
	future_ = std::move(taskHandle.future_);
}

Eugene::TaskHandle& Eugene::TaskHandle::operator=(TaskHandle&& other) noexcept
{
	future_ = std::move(other.future_);
	return *this;
}

Eugene::TaskHandle::TaskHandle(std::future<void>&& future) :
	future_{std::move(future)}
{
}

Eugene::Worker::Worker(ThreadPool& threadPool) :
	threadPool_{threadPool}
{
	// 実行フラグをtrueに
	isRun_.store(true);

	// スレッドを作成してRunを実行する
	thread_ = std::thread{ &Worker::Run,this };
}

Eugene::Worker::~Worker()
{
	// 終了状態にする
	End();

	// join
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
		// タスクを取得
		task_ = std::move(threadPool_.GetTask());

		// タスクを実行
		task_();
	}
}
