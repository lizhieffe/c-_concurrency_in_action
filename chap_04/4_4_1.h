#include <algorithm>
#include <cstdlib>
#include <future>
#include <iostream>
#include <list>

template<typename T>
void PrintList(const std::list<T>& input_list) {
  for (const auto& v : input_list) {
    std::cout << v << ", ";
  }
  std::cout << std::endl;
}

template<typename T>
std::list<T> ParallelQuickSort(std::list<T> input) {
  std::cout << "input: ";
  PrintList(input);

  if (input.size() <= 1) {
    return input;
  }
  std::list<T> result;

  T pivot;
  if (rand() % 2 == 0) {
    pivot = *input.begin();
  } else {
    pivot = input.back();
  }

  auto device_point = std::partition(input.begin(), input.end(), [&](const T& t) {
      return t < pivot; });

  std::list<T> lower_part;
  lower_part.splice(lower_part.begin(), input, input.begin(), device_point);
  std::future<std::list<T>> new_lower_future(std::async(ParallelQuickSort<T>, std::move(lower_part)));

  std::list<T> new_upper(ParallelQuickSort(input));
  std::list<T> new_lower(new_lower_future.get());

  result.splice(result.begin(), new_upper);
  result.splice(result.begin(), new_lower);
  std::cout << "new_upper: ";
  PrintList(new_upper);
  std::cout << "new_lower: ";
  PrintList(new_lower);
  return result;
}

void Run441() {
  std::list<int> numbers {5, 4, 3, 2, 1};
  std::list<int> sorted_numbers = ParallelQuickSort(numbers);
  PrintList(sorted_numbers);
}
