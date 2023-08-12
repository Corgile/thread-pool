#include <iostream>
#include <thread>
#include <memory>

class some_class {
public:
  void foo() {
    std::cout << "hello\n";
  }
};

int main() {
  std::shared_ptr<some_class> a = std::make_shared<some_class>();
  /// 为什么要传入a? 因为执行foo函数的时候需要实例化一个对象
  /// 通过this->foo() 或者instance->foo()来调用foo函数,
  /// 这个 a 相当于告诉了 t 这个this/instance是谁.
  std::thread t(&some_class::foo, a);
  t.join();
  return 0;
}
