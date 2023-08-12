#include <iostream>
#include <thread>

void foo(int &i) {
  i += 1;
}

int main() {
  int refer = 1;
  std::thread t1(foo, std::ref(refer));
  t1.join();
  std::cout << refer << std::endl;
  return 0;
}
