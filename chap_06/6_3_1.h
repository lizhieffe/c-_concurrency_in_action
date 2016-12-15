// Not finished since the impl is relatively simple. The idea is interesting
// though.

#include <assert.h>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <utility>

#include <boost/thread/shared_mutex.hpp>

namespace ccia {

template <typename Key, typename Value>
class BucketType { 
 private:
  using BucketValue = std::pair<Key, Value>;
  using BucketData = std::list<BucketValue>;
  using BucketIterator = typename BucketData::iterator;
  using Hash = std::hash<Key>;

  BucketData bucket_data_;
  mutable boost::shared_mutex shared_mutex_;

 public:
};

void Run631() {
}

}  // namespace ccia
