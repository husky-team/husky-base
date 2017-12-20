// Copyright 2017 Husky Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
