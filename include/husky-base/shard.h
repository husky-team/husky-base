#pragma once

namespace husky {
namespace base {

class Shard {
  public:
    Shard(int local_shard_id, int process_id) : local_shard_id_(local_shard_id), process_id_(process_id) {}

    int GetLocalShardId() { return local_shard_id_; }
    int GetProcessId() { return process_id_; }
  protected:
    int local_shard_id_;
    int process_id_;
};

}  // namespace base
}  // namespace husky
