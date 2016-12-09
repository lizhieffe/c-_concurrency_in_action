#include "spin_lock.h"

namespace ccia {

SpinLock::SpinLock() : atomic_flag_(ATOMIC_FLAG_INIT) {}

SpinLock::~SpinLock() {}

void SpinLock::Lock() {
  while (atomic_flag_.test_and_set()) {}
}

void SpinLock::Unlock() {
  atomic_flag_.clear();
}

}  // namespace ccia
