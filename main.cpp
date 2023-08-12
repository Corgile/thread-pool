#include <iostream>
#include <thread>

void foo(int &i) {
  i += 1;
}

std::thread t1;
void test() {
  int a  = 1;
  t1 = std::thread(foo, std::ref(a));
}
int main() {
  test();
  /**
   * 在Visual Studio下运行会报错：
   * (process 17892) exited with code -1073741819
   * 因为在test函数结束后, a已经被销毁了，此时再在main中去join()的话，
   * t1 已经找不到&a了，因此会出错.
   */
  t1.join();
  return 0;
}
