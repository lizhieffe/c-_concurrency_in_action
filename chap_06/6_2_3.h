#include <assert.h>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace ccia {

template <typename T>
class ThreadsafeQueue {
 private:
  struct Node {
    T value;
    std::unique_ptr<Node> next;
    Node(T input) : value(std::move(input)) {}
    static std::unique_ptr<Node> DummyNode() {
      return std::unique_ptr<Node>(new Node);
    }

   private:
    Node() {}
  };

 public:
  ThreadsafeQueue()
    : dummy_front_(Node::DummyNode()),
      tail_(dummy_front_.get()) {}
  
  void Push(T new_value) {
    {
      std::unique_ptr<Node> new_tail(new Node(std::move(new_value)));
      Node* new_tail_ptr = new_tail.get();
      std::lock_guard<std::mutex> tail_lock_guard(tail_lock_);
      tail_->next = std::move(new_tail);
      tail_ = new_tail_ptr;
      }
    data_cond.notify_one(); 
  }

  std::unique_ptr<T> TryPop() {
    std::unique_ptr<Node> result_node = TryPopHead();
    return result_node == nullptr ?
        nullptr : std::unique_ptr<T>(new T(result_node->value));
  }

  std::unique_ptr<T> WaitAndPop() {
    std::unique_ptr<Node> result_node = WaitAndPopHead();
    return std::unique_ptr<T>(new T(result_node->value));
  }

 private:
  Node* GetTail() {
    std::lock_guard<std::mutex> tail_lock_guard(tail_lock_);
    return tail_;
  }

  std::unique_ptr<Node> PopHead() {
    auto result_node = std::unique_ptr<Node>(dummy_front_->next.release());
    dummy_front_->next = std::move(result_node->next);
    if (dummy_front_->next == nullptr) {
      tail_ = dummy_front_.get();
    }
    return result_node;
  }

  std::unique_ptr<Node> TryPopHead() {
    std::lock_guard<std::mutex> dummy_front_lock_guard(dummy_front_lock_);
    Node* tail = GetTail();
    if(tail != dummy_front_.get()) {
      return PopHead();
    } else {
      return nullptr;
    }
  }

  std::unique_ptr<Node>WaitAndPopHead() {
    std::unique_lock<std::mutex> dummy_front_lock(dummy_front_lock_);
    data_cond.wait(dummy_front_lock,
                   [&]{ return GetTail() != dummy_front_.get(); });
    return PopHead();
  }

  std::unique_ptr<Node> dummy_front_;
  Node* tail_;

  std::mutex dummy_front_lock_;
  std::mutex tail_lock_;

  std::condition_variable data_cond;
};

template <typename T>
void PushAndTryPop(ThreadsafeQueue<T>* queue) {
  int num = 100;
  for (int i = 0; i < num; ++i) {
    queue->Push(1);
  }
  int i = 0;
  while (i < num) {
    if (queue->TryPop() != nullptr) {
      ++i;
      std::cout << "pop i = " << i << std::endl;
    }
  }
}

template <typename T>
void PushAndWaitPop(ThreadsafeQueue<T>* queue) {
  int num = 100;
  for (int i = 0; i < num; ++i) {
    queue->Push(1);
  }
  for (int i = 0; i < num; ++i) {
    queue->WaitAndPop();
    std::cout << "pop i = " << i + 1 << std::endl;
  }
}

void Run623() {
  ThreadsafeQueue<int> queue;
  assert(queue.TryPop() == nullptr);
  queue.Push(1);
  assert(*queue.TryPop() == 1);
  assert(queue.TryPop() == nullptr);
  queue.Push(2);
  queue.Push(3);
  assert(*queue.TryPop() == 2);
  assert(*queue.TryPop() == 3);
  assert(queue.TryPop() == nullptr);

  std::vector<std::thread> threads;

  for (int i = 0; i < 100; ++i) {
    threads.emplace_back(PushAndTryPop<int>, &queue); 
  }
  for (auto& thread : threads) {
    thread.join();
  }

  threads.clear();
  for (int i = 0; i < 100; ++i) {
    threads.emplace_back(PushAndWaitPop<int>, &queue); 
  }
  for (auto& thread : threads) {
    thread.join();
  }
}

}  // namespace ccia
