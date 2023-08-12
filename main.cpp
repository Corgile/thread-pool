#include <iostream>
#include <thread>
#include <memory>
#include <mutex>

int a = 0;
std::mutex mtx;

void add_up() {
  for (int i = 0; i < 1000; ++i) {
    /**
     * 线程安全：多线程环境下不论程序运行多少次，其结果都会和单线程模式下的结果保持一致
     */
    mtx.lock();
    a += 1;
    mtx.unlock();
  }
}


int main() {

  std::thread t1(add_up);
  std::thread t2(add_up);
  t1.join();
  t2.join();
  std::cout << a << std::endl;
  return 0;
}
