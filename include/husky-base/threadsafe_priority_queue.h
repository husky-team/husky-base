#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace husky {
namespace base {

// Store the shared pointer of mailbox event
// Handle smaller priority event first
// Events with same priority should be FIFO
// By now, priority = -1 are system mailbox events, other priority >= 0 are job events
template <typename T>
class ThreadsafePriorityQueue {
 public:
  ThreadsafePriorityQueue(int sch_mode = 0) : sch_mode_(sch_mode) {}
  ~ThreadsafePriorityQueue() = default;
  ThreadsafePriorityQueue(const ThreadsafePriorityQueue&) = delete;
  ThreadsafePriorityQueue& operator=(const ThreadsafePriorityQueue&) = delete;
  ThreadsafePriorityQueue(ThreadsafePriorityQueue&&) = delete;
  ThreadsafePriorityQueue& operator=(ThreadsafePriorityQueue&&) = delete;

  void Push(T elem_ptr) {
    mu_.lock();
    int id = elem_ptr->GetPriority();
    queue_[id].push(elem_ptr);
    if (queue_[id].size() == 1) {
      global_queue_.insert(id);
    }
    mu_.unlock();
    cond_.notify_one();
  }

  T WaitAndPop() {
    auto GetCurrentTime = []() {
      using namespace std::chrono;
      milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
      return ms.count();
    };
    std::unique_lock<std::mutex> lk(mu_);
    cond_.wait(lk, [this] { return !global_queue_.empty(); });
    int64_t current_time = GetCurrentTime();
    int idx = -1;
    int64_t min_time = 0;
    if (*global_queue_.begin() == -1) {
      idx = -1;
    } else {
      // 200 ms
      const int threshold = 5000;
      if (current_time - pre_time_ > threshold) {
        for (auto& value : priority_) {
          value.second = 0;
        }
        pre_time_ = current_time;
      }
      for (auto id : global_queue_) {
        auto& priority = priority_[id];
        if (idx == -1 || priority < min_time) {
          idx = id;
          min_time = priority;
        }
      }
    }
    T elem = queue_[idx].front();
    queue_[idx].pop();
    if (queue_[idx].empty()) {
      global_queue_.erase(idx);
    }
    if (idx != -1 && sch_mode_ == 1) {
      int size = elem->GetSize();
      priority_[idx] += size;
    }
    return elem;
  }

 private:
  // 0 : job_id
  // 1 : fair
  int sch_mode_ = 0;
  int64_t pre_time_;
  std::mutex mu_;
  std::unordered_set<int> global_queue_;
  std::unordered_map<int, std::queue<T>> queue_;
  std::unordered_map<int, int64_t> priority_;
  std::condition_variable cond_;
};

}  // namespace base
}  // namespace husky
