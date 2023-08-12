#include <iostream>
#include <thread>
#include <string>

void print_msg(const std::string &msg) {
  std::cout << msg << "\n";
}

int main() {
  std::thread t1(print_msg, "hello ");
  /// detach: t1在后台运行, main不等待t1 return.
  /// 是main结束, detach()的栈帧也会被收回
  t1.detach();
  return 0;
}
