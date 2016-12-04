#include <iostream>

#include "hierarchical_mutex.h"

HierarchicalMutex::HierarchicalMutex(unsigned long hierarchy_value)
    : hierarchy_value_(hierarchy_value) {}

void HierarchicalMutex::Lock() {
  if (CheckForHierarchyViolation()) {
    internal_mutex_.lock();
    UpdateHierarchyValue();
  }
}

bool HierarchicalMutex::TryLock() {
  if (CheckForHierarchyViolation()) {
    if (internal_mutex_.try_lock()) {
      UpdateHierarchyValue();
      return true;
    } else {
      return false;
    }
  }
  return false;
}

void HierarchicalMutex::Unlock() {
  this_thread_hierarchy_value = previous_hierarchy_value_;
  internal_mutex_.unlock();
}

bool HierarchicalMutex::CheckForHierarchyViolation() {
  if (this_thread_hierarchy_value <= hierarchy_value_) {
    std::cerr << "Current lock hierarchy value (" << this_thread_hierarchy_value
              << ") is not greater than the new lock's hierarchy value ("
              << hierarchy_value_ << ")." << std::endl;
    return false;
  }
  return true;
}

void HierarchicalMutex::UpdateHierarchyValue() {
  previous_hierarchy_value_ = this_thread_hierarchy_value;
  this_thread_hierarchy_value = hierarchy_value_;
}

thread_local unsigned long HierarchicalMutex::this_thread_hierarchy_value{ULONG_MAX};
