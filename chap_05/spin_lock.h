#include <atomic>

namespace ccia {

class SpinLock {
 public:
  SpinLock();
  ~SpinLock();

  SpinLock(const SpinLock&) = delete;
  SpinLock& operator=(const SpinLock&) = delete;

  void Lock();
  void Unlock();

 private:
  std::atomic_flag atomic_flag_;
};

}  // namespace ccia
