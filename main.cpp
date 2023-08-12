#include <iostream>
#include <thread>
#include <memory>

class some_class {
private:
  friend void thread_foo();

  void foo() {
    std::cout << "hello\n";
  }
};


void thread_foo() {
  std::shared_ptr<some_class> a = std::make_shared<some_class>();
  /// 线程访问私有成员: friend
  std::thread t(&some_class::foo, a);
  t.join();
}

int main() {
  thread_foo();
  return 0;
}
