#include <iostream>
#include <thread>
#include <mutex>

/// 单例logger, 并不严谨
class logger;
static logger *m_instance = nullptr;
static std::once_flag once;
class logger {
private:
  logger() = default;

public:

  /// 禁用拷贝构造
  logger(const logger &other) = delete;

  /// 禁用拷贝构造(重载)
  logger &operator=(const logger &other) = delete;

  /// 饿汉单例: 提前声明, 需要时实例化
  static logger &instance() {
    std::call_once(once, []() {
      if (!m_instance) m_instance = new logger();
    });
    return *m_instance;
  }

  void info(const std::string &msg) {
    std::cout << this << " " << __TIMESTAMP__ << " " << msg << std::endl;
  }
};

void print_msg(const std::string &msg) {
  logger::instance().info(msg);
}

int main() {
  /// 多线程访问get_instance可能造成instance被实例化多次
  ///
  std::thread t1(print_msg, "error 1\n");
  std::thread t2(print_msg, "error 2\n");
  std::thread t3(print_msg, "error 3\n");
  std::thread t4(print_msg, "error 4\n");
  std::thread t5(print_msg, "error 5\n");
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();
  return 0;
}
