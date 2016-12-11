#include <assert.h>
#include <atomic>
#include <iostream>
#include <thread>

namespace ccia {

std::atomic<bool> x, y;
std::atomic<bool> z;

void WriteXThenY() {
  x.store(true, std::memory_order_relaxed);
  y.store(true, std::memory_order_relaxed);
}

void ReadYThenX() {
  while (!y.load(std::memory_order_relaxed));
  assert(x.load(std::memory_order_relaxed) == true);
}

void Run533() {
  for (int i = 0; i < 10; ++i) {
    std::thread write_x_then_y(WriteXThenY);
    std::thread read_y_then_x(ReadYThenX);
    write_x_then_y.join();
    read_y_then_x.join();
  }
}

}  // namespace ccia
