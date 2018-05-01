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

#include "husky-base/bin_stream.h"
#include "husky-base/shard.h"
#include "husky-base/threadsafe_priority_queue.h"

namespace husky {
namespace base {

// Mailbox event types
enum MailboxEventType : int32_t { RecvComm = 1, SetRecvHandler = 2, SendComm = 3, Shutdown = 4 };

// Typedefs for send/recv handler
typedef std::function<void(Shard shard, int channel_id, BinStream* payload)> MailboxSendHandlerType;
typedef std::function<void(int local_shard_id, std::shared_ptr<BinStream> payload)> MailboxRecvHandlerType;

class MailboxEventBase {
 public:
  MailboxEventBase(MailboxEventType type, int priority) : type_(type), priority_(priority) {}

  MailboxEventType GetType() { return type_; }
  int GetPriority() { return priority_; }
  virtual int GetSize() { return 0; }

 private:
  MailboxEventType type_;
  int priority_;
};

class MailboxEventRecvComm : public MailboxEventBase {
 public:
  // TODO(czk) we should not let the mailbox event loop deserialize data in channel?
  MailboxEventRecvComm(int local_shard_id, int channel_id, std::shared_ptr<BinStream> bin_stream)
      : MailboxEventBase(MailboxEventType::RecvComm, -1),
        local_shard_id_(local_shard_id),
        channel_id_(channel_id),
        bin_stream_(bin_stream) {}
  int GetLocalShardId() { return local_shard_id_; }
  int GetChannelId() { return channel_id_; }
  std::shared_ptr<BinStream> GetBinStream() { return bin_stream_; }

 private:
  int local_shard_id_;
  int channel_id_;
  std::shared_ptr<BinStream> bin_stream_;
};

class MailboxEventSetRecvHandler : public MailboxEventBase {
 public:
  MailboxEventSetRecvHandler(int channel_id, MailboxRecvHandlerType* handler)
      : MailboxEventBase(MailboxEventType::SetRecvHandler, -1), channel_id_(channel_id), handler_(handler) {}
  int GetChannelId() { return channel_id_; }
  MailboxRecvHandlerType* GetHandler() { return handler_; }

 private:
  int channel_id_;
  MailboxRecvHandlerType* handler_;
};

class MailboxEventSendComm : public MailboxEventBase {
 public:
  MailboxEventSendComm(int process_id, int local_shard_id, int channel_id, std::shared_ptr<BinStream> payload,
                       int priority)
      : MailboxEventBase(MailboxEventType::SendComm, priority),
        process_id_(process_id),
        local_shard_id_(local_shard_id),
        channel_id_(channel_id),
        payload_(payload) {}

  int GetProcessId() { return process_id_; }
  int GetLocalShardId() { return local_shard_id_; }
  int GetChannelId() { return channel_id_; }
  int GetSize() override { return payload_->size(); }
  std::shared_ptr<BinStream> GetPayload() { return payload_; }

 private:
  int process_id_;
  int local_shard_id_;
  int channel_id_;
  std::shared_ptr<BinStream> payload_;
};

class MailboxEventShutdown : public MailboxEventBase {
 public:
  MailboxEventShutdown() : MailboxEventBase(MailboxEventType::Shutdown, -1) {}
};

typedef ThreadsafePriorityQueue<std::shared_ptr<MailboxEventBase>> MailboxEventQueue;
typedef std::shared_ptr<MailboxEventQueue> MailboxEventQueuePtr;

}  // namespace base
}  // namespace husky
