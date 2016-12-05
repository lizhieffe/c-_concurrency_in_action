#include <chrono>
#include <deque>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

int kTaskNum = 0;

std::mutex m;
std::deque<std::packaged_task<void()>> tasks;

bool IsGuiShutdown() {
  static int count = 0;
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  return ++count > kTaskNum;
}

void GetAndProcessGuiMessage() {
  std::cout << __func__ << std::endl;
}

void GuiThread() {
  while (!IsGuiShutdown()) {
    GetAndProcessGuiMessage();
    std::packaged_task<void()> task;
    {
      std::lock_guard<std::mutex> lg(m);
      std::cout << "tasks size = " << tasks.size() << std::endl;
      if (tasks.empty()) {
        continue;
      }
      task = std::move(tasks.front());
      tasks.pop_front();
    }
    task();
  }
}

template<typename Func>
std::future<void> AddTaskForGuiThread(Func f) {
  std::cout << __func__ << std::endl;
  std::packaged_task<void()> task(f);
  std::future<void> future = task.get_future();
  std::lock_guard<std::mutex> ul(m);
  tasks.push_back(std::move(task));
  return future;
}

void GuiTaskFunctionPointer() {
  std::cout << __func__ << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void GuiTaskCallableObject() {
  std::cout << __func__ << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

struct GuiTaskStruct {
  void operator() () {
    std::cout << __func__ << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
};

void Run422() {
  std::cout << std::endl << std::endl << "4.2.2" << std::endl;
  std::future<void> f1 = AddTaskForGuiThread(&GuiTaskFunctionPointer);
  std::future<void> f2 = AddTaskForGuiThread(std::bind(&GuiTaskCallableObject));
  std::future<void> f3 = AddTaskForGuiThread(GuiTaskStruct());
  std::future<void> f4 = AddTaskForGuiThread([]() {
      std::cout << "Lambda function." << std::endl;
      });
  kTaskNum = 4;
  GuiThread();
  f1.get();
  f2.get();
  f3.get();
  f4.get();
}
