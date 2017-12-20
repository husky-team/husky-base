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

#include <string>
#include <thread>

#include "zmq.hpp"

#include "husky-base/bin_stream.h"
#include "husky-base/shard.h"

namespace husky {
namespace base {

class MailboxRecver {
 public:
  MailboxRecver(const std::string& bind_addr, const std::string& connect_addr, zmq::context_t* zmq_context);
  ~MailboxRecver();

 protected:
  std::unique_ptr<std::thread> thread_;
  zmq::context_t* zmq_context_;
  std::string bind_addr_;
  std::string connect_addr_;
};

}  // namespace base
}  // namespace husky
