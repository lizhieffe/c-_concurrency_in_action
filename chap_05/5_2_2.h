#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

#include "spin_lock.h"

namespace ccia {

SpinLock spin_lock;
int num = 0;

void PlusThenMinus() {
  spin_lock.Lock();
  num++;
  std::cout << "num after plus: " << num << std::endl;
  num--;
  std::cout << "num after minus: " << num << std::endl;
  spin_lock.Unlock();
}

void Run522() {
  std::vector<std::thread> threads;
  threads.emplace_back(&PlusThenMinus);
  threads.emplace_back(&PlusThenMinus);
  threads.emplace_back(&PlusThenMinus);
  threads.emplace_back(&PlusThenMinus);
  threads.emplace_back(&PlusThenMinus);
  threads.emplace_back(&PlusThenMinus);
  threads.emplace_back(&PlusThenMinus);
  threads.emplace_back(&PlusThenMinus);
  
  for (auto& thread : threads) {
    thread.join();
  }
}

}  // namespace ccia
