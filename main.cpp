#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

class thread_pool {
public:
  explicit thread_pool(const int &pool_size) : m_stopped(false) {
    this->m_threads.reserve(pool_size);
    for (int i = 0; i < pool_size; ++i) {
      this->m_threads.emplace_back([this]() {
        while (true) {
          std::unique_lock<std::mutex> u_lock(this->m_mtx);
          m_condition.wait(u_lock, [this]() {
            return this->m_stopped || (!this->m_tasks.empty());
          });
          if (m_stopped && m_tasks.empty()) return;// in lambda
          auto task = this->m_tasks.front();
          this->m_tasks.pop();
          u_lock.unlock();
          task();
        }
      });
    }
  }

  ~thread_pool() {
    {
      std::unique_lock<std::mutex> lock(m_mtx);
      m_stopped = true;
    }
    m_condition.notify_all();
    for (auto &item: m_threads) {
      item.join();
    }
  }

  template<class Func, typename... Param>
  bool add_task(Func &&func, Param &&...param) {
    bool succeed{false};
    std::function<void()> task = std::bind(std::forward<Func>(func), std::forward<Param>(param)...);
    {
      std::unique_lock<std::mutex> u_lock(this->m_mtx);
      succeed = (bool) this->m_tasks.emplace(std::move(task));
    }
    this->m_condition.notify_one();
    return succeed;
  }

private:
  std::vector<std::thread> m_threads;
  std::queue<std::function<void()>> m_tasks;
  std::mutex m_mtx;
  std::condition_variable m_condition;
  bool m_stopped;
};

int main() {
  /// 多线程访问get_instance可能造成instance被实例化多次
  thread_pool pool(5);
  for (int i = 0; i < 5; ++i) {
    std::ignore = pool.add_task([i]() {
      std::cout << __TIMESTAMP__ << "  Task [" << i + 1 << "] starting..." << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << __TIMESTAMP__ << "  Task [" << i + 1 << "] done" << std::endl;
    });
  }
  return 0;
}
