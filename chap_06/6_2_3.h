#include <assert.h>
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
  ThreadsafeQueue() : head_(Node::DummyNode()), tail_(head_.get()) {}
  
  void Push(T new_value) {
    std::unique_ptr<Node> new_tail(new Node(std::move(new_value)));
    Node* new_tail_ptr = new_tail.get();
    std::lock_guard<std::mutex> tail_lock_guard(tail_lock_);
    tail_->next = std::move(new_tail);
    tail_ = new_tail_ptr;
  }

  std::unique_ptr<T> TryPop() {
    std::unique_ptr<Node> result_node;
    {
      std::lock_guard<std::mutex> head_lock_guard(head_lock_);
      Node* tail = nullptr;
      {
        std::lock_guard<std::mutex> tail_lock_guard(tail_lock_);
        tail = tail_;
      }
      if(tail != head_.get()) {
        result_node = std::unique_ptr<Node>(head_->next.release());
        head_->next = std::move(result_node->next);
        if (head_->next == nullptr) {
          tail_ = head_.get();
        }
      }
    }
    return result_node == nullptr ?
        nullptr : std::unique_ptr<T>(new T(result_node->value));
  }

 private:
  std::unique_ptr<Node> head_;
  Node* tail_;

  std::mutex head_lock_;
  std::mutex tail_lock_;
};

template <typename T>
void PushAndPop(ThreadsafeQueue<T>* queue) {
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
    threads.emplace_back(PushAndPop<int>, &queue); 
  }
  for (auto& thread : threads) {
    thread.join();
  }
}

}  // namespace ccia
