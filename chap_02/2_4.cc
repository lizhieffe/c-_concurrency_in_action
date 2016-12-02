#include <algorithm>
#include <functional>
#include <numeric>
#include <iostream>
#include <thread>
#include <vector>

template<typename Iterator, typename T>
struct AccumulateBlock {
  void operator()(Iterator first, Iterator last, T& result) {
    std::cout << "thread id is running: " << std::this_thread::get_id() << std::endl;
    result = std::accumulate(first, last, result);
  }
};

template<typename Iterator, typename T>
T ParallelAccumulate(Iterator first, Iterator last, T init) {
  const unsigned long length = std::distance(first, last);
  if (length <= 0) {
    return init;
  }

  const unsigned long min_per_thread = 2;
  const unsigned long max_threads
      = (length + min_per_thread + 1) / min_per_thread;
  const unsigned long hardward_threads = std::thread::hardware_concurrency();
  
  const unsigned long num_threads
      = std::min(hardward_threads <= 0 ? 2 : hardward_threads, max_threads);
  const unsigned long block_size = length / num_threads;
  std::cout << "block_size = " << block_size << std::endl;
  std::cout << "num_threads = " << num_threads << std::endl;

  std::vector<T> results(num_threads);
  std::vector<std::thread> threads(num_threads - 1);

  Iterator block_start = first;
  for (unsigned long i = 0; i < num_threads - 1; ++i) {
    Iterator block_end = block_start;
    std::advance(block_end, block_size);
    threads[i] = std::thread(AccumulateBlock<Iterator, T>(), block_start,
                             block_end, std::ref(results[i]));
    block_start = block_end;
  }
  AccumulateBlock<Iterator, T>()(block_start, last, std::ref(results[num_threads - 1]));
  std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

  return std::accumulate(results.begin(), results.end(), init);
}

int main() {
  std::vector<int> numbers {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int result = ParallelAccumulate<std::vector<int>::const_iterator, int>(
                   numbers.begin(), numbers.end(), 0);
  std::cout << "final result = " << result << std::endl;
}
