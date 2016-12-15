#include <atomic>
#include <future>
#include <list>
#include <thread>
#include <vector>

template <typename T>
class ThreadSafeStack<T>;

template <typename T>
struct Sorter {
  struct ChunkToSort {
    std::list<T> data;
    std::promise<std::list<T>> promise;
  };

  ThreadSafeStack<ChunkToSort> chunks;
  std::vector<std::thread> threads;
  const unsigned max_thread_count;
  std::atomic<bool> end_of_data;

  Sorter()
    : max_thread_count(std::thread::hardware_concurrency() - 1),
      end_of_data(false) {}

  ~Sorter() {
    end_of_data = true;
    for (unsigned i = 0; i < threads.size(); ++i) {
      threads[i].join();
    }
  }
              
  void TrySortChunk() {
    std::unique_ptr<ChunkToSort> chunk = chunks.pop();
    if (chunk != nullptr) {
      SortChunk(chunk);
    }
  }
};
