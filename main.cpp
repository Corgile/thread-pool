#include <iostream>
#include <thread>
#include <mutex>

int shared_data = 0;
std::timed_mutex mtx;
#define max_wait 6050
#define job_cost 2
#define round 2

void add_up() {
  /// try_lock_for: 超过设定时间获取不到锁直接返回
  std::unique_lock<std::timed_mutex> u_lock(mtx, std::defer_lock);
  if (u_lock.try_lock_for(std::chrono::microseconds(max_wait))) {
    for (int i = 0; i < round; ++i) {
      /// 每操作一次 shared_data 需要花费2秒，总共执行2次, 因此每个线程执行完成需要4秒。
      /// 但是每个线程的 u_lock 只被允许等待 timed_mutex 互斥锁不超过3秒，
      /// 所以当前线程在第二次对 shared_data 操作完成前，另一个线程的u_lock已经等待超时并返回
      /// 因此这里的t1 t2两个线程一定只有一个线程能获得锁并完成2次
      /// 对shared_data的操作，所以最终shared_data=3, 两次打印current_thread也是相同的.
      std::this_thread::sleep_for(std::chrono::seconds(job_cost));
      std::cout << "Thread [" << std::this_thread::get_id() << "] is operating shared_data" << std::endl;
      shared_data++;
    }
  }
}

int main() {

  std::thread t1(add_up);
  std::thread t2(add_up);
  t1.join();
  t2.join();
  std::cout << shared_data << std::endl;
  return 0;
}
