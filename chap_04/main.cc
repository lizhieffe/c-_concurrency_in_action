#include <cassert>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>

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
}
