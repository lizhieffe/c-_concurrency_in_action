#include <cassert>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>

#include "4_2_1.h"
#include "4_2_2.h"
#include "threadsafe_queue.h"

std::list<int> data_list;

std::mutex lock;
std::condition_variable data_cond;
bool data_finished = false;

bool HasMoreDataToPrepare() {
  static int i = 0;
  return ++i <= 10;
}

void DataPreparationThread() {
  static int data = 0;
  while (HasMoreDataToPrepare()) {
    // Mimic preparing data.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::lock_guard<std::mutex> lg(lock);
    data_list.push_back(++data);
    data_cond.notify_one();
  }

  // Updata data_finished.
  std::lock_guard<std::mutex> lg(lock);
  data_finished = true;
  data_cond.notify_one();
}

void DataProcessingThread() {
  while (true) {
    std::unique_lock<std::mutex> ul(lock);
    data_cond.wait(ul, [] () { return !data_list.empty() || data_finished; });
    if (!data_list.empty()) {
      int data = data_list.front();
      data_list.pop_front();
      ul.unlock();
      // Mimic processing data.
      std::cout << "Processing data: " << data << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } else {
      break;
    }
  }
  std::cout << "Finished processing all the data!" << std::endl;
}

int main() {
  // 4.1.1
  std::thread data_preparation_thread(DataPreparationThread);
  std::thread data_processing_thread(DataProcessingThread);
  data_preparation_thread.join();
  data_processing_thread.join();

  // 4.1.2
  std::cout << std::endl;
  ThreadsafeQueue<int> threadsafe_queue;
  int value;
  assert(threadsafe_queue.Empty());  
  assert(threadsafe_queue.TryPop(&value) == false);
  assert(threadsafe_queue.TryPop() == nullptr);
  threadsafe_queue.Push(3);
  assert(*threadsafe_queue.TryPop() == 3);
  std::thread threadsafe_queue_data_push_thread([&threadsafe_queue]() {
      for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        threadsafe_queue.Push(i);
      }
      });
  std::thread threadsafe_queue_data_pop_thread([&threadsafe_queue]() {
      for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        assert(*threadsafe_queue.WaitAndPop() == i);
      }
      });
  threadsafe_queue_data_push_thread.join();
  threadsafe_queue_data_pop_thread.join();

  // 4.2.1
  std::cout << std::endl;
  X x;
  auto future_1 = std::async(&X::foo, &x, 42, "hello"); 
  auto future_2 = std::async(&X::bar, x, "goodbye");
  future_1.get();
  std::cout << "future_2 get is: " << future_2.get() << std::endl;
  auto future_3 = std::async(Y(), 3);
  Y y;
  auto future_4 = std::async(std::ref(y), 4);
  std::cout << "future_3 get is: " << future_3.get() << std::endl;
  std::cout << "future_4 get is: " << future_4.get() << std::endl;

  // 4.2.2
  Run422();
}
