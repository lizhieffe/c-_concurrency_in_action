#include <chrono>
#include <iostream>
#include <string>
#include <thread>

void do_some_work() {
  std::cout << __func__ << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void do_some_work_with_params(int number) {
  std::cout << __func__ << " " << number << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void do_some_work_with_ref(const std::string& str) {
  std::cout << __func__ << " " << str << std::endl;
  std::cout << "str address is: " << &str << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

class ThreadGuard {
 public:
  ThreadGuard(std::thread* thread) : thread_(thread) {}

  ~ThreadGuard() {
    if (thread_->joinable()) {
      thread_->join();
    }  
  }

  ThreadGuard(const ThreadGuard&) = delete;
  ThreadGuard& operator=(const ThreadGuard&) = delete;

 private:
  std::thread* thread_;
};

class FunctionObject {
 public:
  void do_some_work_in_class(const std::string str) {
    std::cout << __func__ << " " << str << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
};

int main() {
  // A plain function.
  std::thread thread_1(do_some_work);
  ThreadGuard thread_1_guard(&thread_1);

  // lambda function.
  std::thread thread_2([]() {
      do_some_work();
    });
  thread_2.join();

  // Detach.
  std::thread thread_3(do_some_work);
  thread_3.detach();

  // A plain function with params.
  int number = 4;
  std::thread thread_4(do_some_work_with_params, number);
  ThreadGuard thread_4_guard(&thread_4);

  // lambda function with params.
  number = 5;
  std::thread thread_5([number]() {
      do_some_work_with_params(number);
    });
  thread_5.join();

  // A plain function with ref. 
  std::string str{"abc"};
  std::thread thread_6(do_some_work_with_ref, std::ref(str));
  ThreadGuard thread_6_guard(&thread_6);
  std::cout << "str address is: " << &str << std::endl;

  // Call object function.
  FunctionObject fo;
  std::thread thread_7(&FunctionObject::do_some_work_in_class, &fo, str);
  thread_7.join();

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  return 0;
}
