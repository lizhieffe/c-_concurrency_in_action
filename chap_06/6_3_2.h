#include <assert.h>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace ccia {

template <typename T>
class ThreadSafeList {
 public:
  ThreadSafeList() {}
  ~ThreadSafeList() {
    RemoveIf([](Node const&) { return true; }); 
  }

  void PushFront(const T& data) {
    std::unique_ptr<Node> new_node(new Node(data));
    std::lock_guard<std::mutex> lg(dummy_head_.mu);
    new_node->next = std::move(dummy_head_.next);
    dummy_head_.next = std::move(new_node);
  }

  template <typename Function>
  void ForEach(Function f) {
    Node* curr = &dummy_head_;
    std::unique_lock<std::mutex> curr_lock(curr->mu);
    while (Node* next = curr->next.get()) {
      std::unique_lock<std::mutex> next_lock(next->mu);
      curr_lock.unlock();
      f(*next->data);
      curr = next;
      curr_lock = std::move(next_lock);
    }
  }

  template <typename Function>
  std::shared_ptr<T> FindFirstIf(Function f) {
    Node* curr = &dummy_head_;
    std::unique_lock<std::mutex> curr_lock(curr->mu);
    while (Node* next = curr->next.get()) {
      std::unique_lock<std::mutex> next_lock(next->mu);
      curr_lock.unlock();
      if (f(*next->data)) {
        return next->data;
      } else {
        curr = next;
        curr_lock = std::move(next_lock);
      }
    }
  } 

  template <typename Predict>
  void RemoveIf(Predict p) {
    Node* curr = &dummy_head_;
    std::unique_lock<std::mutex> curr_lock(curr->mu);
    while (Node* next = curr->next.get()) {
      std::unique_lock<std::mutex> next_lock(next->mu);
      if (p(*next->data)) {
        std::unique_ptr<Node> old_next = std::move(curr->next);
        curr->next = std::move(old_next->next);
        next_lock.unlock();
      } else {
        curr_lock.unlock();
        curr = next;
        curr_lock = std::move(next_lock);
      }
    }
  }

 private:
  struct Node {
    Node() {}
    Node(const T& data) : data(new T(data)) {}

    std::mutex mu;
    std::shared_ptr<T> data;
    std::unique_ptr<Node> next;
  };

  Node dummy_head_;

};

void Run632() {
  ThreadSafeList<int> list;
  int test_size = 100;
  std::vector<std::thread> threads;
  for (int i = 0; i < test_size; ++i) {
    threads.emplace_back([&list, i]() { list.PushFront(i); });
  }
  for (int i = 0; i < test_size; ++i) {
    threads.emplace_back([&list, i]() { 
        list.ForEach([](const int input) { 
            std::cout << input << std::endl;
        });
    });
  }
  for (int i = 0; i < test_size; ++i) {
    threads.emplace_back([&list, i]() { 
        list.RemoveIf([](const int) { 
            return true;
        });
    });
  }
  for (auto& thread : threads) {
    thread.join();
  }
}

}  // namespace ccia
