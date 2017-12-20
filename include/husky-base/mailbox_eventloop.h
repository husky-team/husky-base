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

#include <functional>
#include <map>
#include <thread>

#include "zmq.hpp"

#include "husky-base/bin_stream.h"
#include "husky-base/mailbox_adress_book.h"
#include "husky-base/shard.h"

namespace husky {
namespace base {

class MailboxEventLoop {
 public:
  typedef std::function<void(int local_shard_id, int progress, BinStream*)> RecvAvailableHandlerType;
  typedef std::function<void(int progress)> RecvCompleteHandlerType;
  typedef std::function<void(Shard shard, int channel_id, int progress, BinStream*)> SendHandlerType;
  typedef std::function<void(int channel_id, int progress)> SendCompleteHandlerType;

  explicit MailboxEventLoop(zmq::context_t* zmq_context);
  ~MailboxEventLoop();

  // There following two methods are not thread-safe, since we assume that they are set up at the very beginning. If
  // they need to be changed later, we need to provide thread-safe ways
  void OnSend(SendHandlerType handler);
  void OnSendComplete(SendCompleteHandlerType handler);

 protected:
  std::unique_ptr<std::thread> thread_;
  zmq::context_t* zmq_context_;
  std::map<int, RecvAvailableHandlerType> recv_available_handlers_;
  std::map<int, RecvCompleteHandlerType> recv_complete_handlers_;
  SendHandlerType send_handler_;
  SendCompleteHandlerType send_complete_handler_;
};

}  // namespace base
}  // namespace husky
