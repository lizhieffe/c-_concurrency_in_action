#include <climits>
#include <mutex>

class HierarchicalMutex {
 public:
  explicit HierarchicalMutex(unsigned long hierarchy_value);

  void Lock();
  bool TryLock();
  void Unlock();

 private:
  // Returns false if there is hierarchy violation for the mutex lock.
  bool CheckForHierarchyViolation();

  void UpdateHierarchyValue();

  std::mutex internal_mutex_;
  const unsigned long hierarchy_value_;
  unsigned long previous_hierarchy_value_ = 0;

  static thread_local unsigned long this_thread_hierarchy_value;
};

