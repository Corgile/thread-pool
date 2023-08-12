#include <iostream>
#include <thread>
#include <string>

void print_msg(const std::string &msg) {
  std::cout << msg << "\n";
}

int main() {
  std::thread t1(print_msg, "hello ");
  /// join: 告诉main是否要等待t1 return
  t1.join();
  return 0;
}
