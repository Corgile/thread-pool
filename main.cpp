#include <iostream>
#include <thread>

std::thread t1;

void foo(int *i) {
  *i += 1;
  /// VS: 输出-572662306; Clion: 程序卡死
  std::cout << *i << std::endl;
}


int main() {
  int *ptr = new int(1);
  t1 = std::thread(foo, ptr);
  delete ptr;
  ptr = nullptr;
  /// 线程启动前，某些变量被销毁，导致未定义错误
  t1.join();
  return 0;
}
