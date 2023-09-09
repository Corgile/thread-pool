#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <future>
#include <string>

struct Result {
	std::string str;
	int value{};

	void print() const {
		std::cout << "Result={name: " << str << ", id: " << value << "}\n";
	}
};

class ThreadPool {
public:
	explicit ThreadPool(int32_t num_threads) : m_stopped(false), m_capacity(num_threads) {
		for (size_t i = 0; i < m_capacity; ++i) {
			m_threads.emplace_back([this] {
				while (true) {
					std::function<std::future<Result>()> task;
					{
						std::unique_lock<std::mutex> lock(m_mutex);
						condition.wait(lock, [this] { return m_stopped || !m_tasks.empty(); });
						if (m_stopped && m_tasks.empty()) return;
						task = std::move(m_tasks.front());
						m_tasks.pop();
					}
					std::future<Result> future = task();
					Result result = future.get();
					std::lock_guard<std::mutex> lock(m_result_mutex);
					m_results.push_back(result);
				}
			});
		}
	}

	~ThreadPool() {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_stopped = true;
		lock.unlock();
		condition.notify_all();
		for (auto& thread: m_threads) {
			if (thread.joinable())thread.join();
		}
	}

	template<class Func, class... Args>
	std::future<Result> enqueue(Func&& func, Args&& ... args) {
		auto task = [&]() -> std::future<Result> {
			std::packaged_task<Result()> packagedTask([&] { return func(std::forward<Args>(args)...); });
			std::future<Result> future = packagedTask.get_future();
			packagedTask();
			return future;
		};
		std::future<Result> future = task();
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_tasks.emplace(std::move(task));
		}
		condition.notify_one();
		return future;
	}

	std::vector<Result> get_results() {
		std::lock_guard<std::mutex> lock(m_result_mutex);
		return m_results;
	}

private:
	std::vector<std::thread> m_threads;
	std::queue<std::function<std::future<Result>()>> m_tasks;
	std::condition_variable condition;
	std::mutex m_mutex;
	bool m_stopped;
	int32_t m_capacity;

	/// 事实上有feature就能拿到返回结果了，但却是阻塞的.
	/// 这里将结果存vector时满足异步获取结果的需求
	std::vector<Result> m_results;
	std::mutex m_result_mutex;
};

Result MyTask(const std::string& message, int id) {
	std::cout << "\033[" << id << ";0HThread[" << id << "] starting...";
	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::cout << "\033[" << id << ";25HExecution done.\n";
	return {message, id << 1};
}

int main() {
	system("chcp 65001 > nul");
	ThreadPool pool(4);

	std::vector<std::future<Result>> futures;

	futures.reserve(4);
	for (int i = 1; i <= 4; ++i) {
		futures.emplace_back(pool.enqueue(MyTask, "Hello", i));
	}

	std::this_thread::sleep_for(std::chrono::seconds(10));
	for (auto& future: futures) {
		Result result = future.get();
		result.print();
	}
	auto results{pool.get_results()};
	for (const auto& item: results) {
		item.print();
	}

	return 0;
}
