// Copyright 2016 Husky Team
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

#include <condition_variable>
#include <functional>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "zmq.hpp"

#include "husky-base/bin_stream.h"
#include "husky-base/mailbox_types.h"
#include "husky-base/shard.h"

namespace husky {
namespace base {

// Endpoint for sending and recieving communication. This class is not thread-safe. A Mailbox instance is supposed to be
// used by the same thread.
class Mailbox {
 public:
  explicit Mailbox(zmq::context_t* zmq_context);
  virtual ~Mailbox();

  /// \brief Send outgoing communication to a specific thread.
  ///
  /// This method can be used multiple times to send multiple BinStreams. After
  /// that, the `send_complete` method should be used to indicate the end of this
  /// batch of communication.
  ///
  /// @param shard The destination worker shard.
  /// @param channel_id ID of the Channel for the communication.
  /// @param progress Progress of the communication. Progress should always
  ///        be increasing.
  /// @param payload The actual communication in the form of BinStream. This method takes over the ownership of the
  /// payload.
  void Send(Shard shard, int channel_id, BinStream* payload);

  /// \brief Set the handler for new incoming communication
  ///
  /// The handler will be apply once there's new incoming communication available
  /// (in the form of BinStream).
  ///
  /// @param handler The handler to use
  void OnRecv(int channel_id, MailboxRecvHandlerType handler);

 protected:
  zmq::context_t* zmq_context_;
  std::unique_ptr<zmq::socket_t> socket_;
};

}  // namespace base
}  // namespace husky
