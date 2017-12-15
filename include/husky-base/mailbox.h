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
#include <string>
#include <thread>
#include <functional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "zmq.hpp"

#include "husky-base/bin_stream.h"
#include "husky-base/shard.h"

namespace husky {
namespace base {

class Mailbox {
 public:
  typedef std::function<void(int progress, BinStream*)> RecvAvailableHandlerType;
  typedef std::function<void(int progress)> RecvCompleteHandlerType;

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
  /// @param bin_stream The actual communication in the form of BinStream.
  void Send(Shard shard, int channel_id, int progress, BinStream* bin_stream);

  /// \brief Indicate that a round of outgoing communication finishes.
  ///
  /// This method must be called after a round of outgoing communication.
  ///
  /// @param channel_id Channel of the communication.
  /// @param progress Progress of the corresponding Channel.
  void SendComplete(int channel_id, int progress);

  /// \brief Set the handler for new incoming communication
  ///
  /// The handler will be apply once there's new incoming communication available
  /// (in the form of BinStream).
  ///
  /// @param handler The handler to use
  void SetRecvAvailableHandler(int channel_id, RecvAvailableHandlerType handler);

  /// \brief Set the handler at the completion incoming communication
  ///
  /// The handler will be used once the communication of a specific (channel_id, progress)
  /// pair finishes.
  ///
  /// @param handler The handler to use
  void SetRecvCompleteHandler(int channel_id, RecvCompleteHandlerType handler);
 protected:
  zmq::context_t* zmq_context_;
  std::unique_ptr<zmq::socket_t> socket_;
};

}  // namespace base
}  // namespace husky
