#include <algorithm>
#include <cassert>
#include <list>
#include <mutex>
#include <thread>

std::mutex lock;
int num = 0;

void Add() {
  std::lock_guard<std::mutex> guard(lock);
  ++num;
}

int main() {
  std::list<std::thread> threads;
  for (int i = 0; i < 10; ++i) {
    threads.emplace_back(Add);
  }
  std::for_each(threads.begin(), threads.end(), mem_fn(&std::thread::join));
  assert(num == 10);
}
