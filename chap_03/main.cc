#include <algorithm>
#include <cassert>
#include <list>
#include <mutex>
#include <thread>

#include "hierarchical_mutex.h"

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

  // Test HierarchicalMutex.
  HierarchicalMutex hierarchical_mutex_1000(1000);
  HierarchicalMutex hierarchical_mutex_100(100);
  HierarchicalMutex hierarchical_mutex_10(10);
  HierarchicalMutex hierarchical_mutex_1(1);

  assert(hierarchical_mutex_100.TryLock());
  assert(hierarchical_mutex_1000.TryLock() == false);
  assert(hierarchical_mutex_10.TryLock());
  assert(hierarchical_mutex_1.TryLock());
  assert(hierarchical_mutex_1000.TryLock() == false);

  hierarchical_mutex_1.Unlock();
  hierarchical_mutex_10.Unlock();
  hierarchical_mutex_100.Unlock();
}
