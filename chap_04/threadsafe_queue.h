#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

template <class T>
class ThreadsafeQueue {
 public:
  ThreadsafeQueue() {}

  ThreadsafeQueue(const ThreadsafeQueue&) = delete;
  ThreadsafeQueue& operator=(const ThreadsafeQueue&) = delete;

  void Push(T new_value) {
    std::cout << "Pushing value: " << new_value << std::endl;
    std::lock_guard<std::mutex> ml(mutex_);
    data_queue_.push(new_value);
    data_cond_.notify_one();
  }

  bool TryPop(T* value) {
    std::lock_guard<std::mutex> ml(mutex_);
    if (data_queue_.empty()) {
      std::cout << "Currently no data to pop." << std::endl;
      return false;
    } else {
      *value = data_queue_.front();
      data_queue_.pop();
      std::cout << "Poping data: " << *value << std::endl;
      return true;
    }
  }

  std::shared_ptr<T> TryPop() {
    std::lock_guard<std::mutex> ml(mutex_);
    if (data_queue_.empty()) {
      std::cout << "Currently no data to pop." << std::endl;
      return nullptr; 
    } else {
      auto value = std::make_shared<T>(data_queue_.front());
      data_queue_.pop();
      std::cout << "Poping data: " << *value << std::endl;
      return value;
    }
  }

  void WaitAndPop(T* value) {
    while (true) {
      std::unique_lock<std::mutex> ml(mutex_);
      data_cond_.wait(ml, [this]() { return !data_queue_.empty(); });
      if (!data_queue_.empty()) {
        *value = data_queue_.top();
        data_queue_.pop();
        std::cout << "Poping data: " << *value << std::endl;
        break;
      }
    }
  }

  std::shared_ptr<T> WaitAndPop() {
    while (true) {
      std::unique_lock<std::mutex> ml(mutex_);
      data_cond_.wait(ml, [this]() { return !data_queue_.empty(); });
      if (!data_queue_.empty()) {
        auto value = std::make_shared<T>(data_queue_.front());
        data_queue_.pop();
        std::cout << "Poping data: " << *value << std::endl;
        return value;
      }
    }
  }

  bool Empty() const {
    std::lock_guard<std::mutex> ml(mutex_);
    return data_queue_.empty();
  }

 private:
  mutable std::mutex mutex_;
  std::queue<T> data_queue_;
  std::condition_variable data_cond_;
};
