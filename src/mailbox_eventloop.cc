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

#include <memory>

#include "husky-base/mailbox_eventloop.h"

#include "husky-base/zmq_helpers.h"

namespace husky {
namespace base {

MailboxEventLoop::MailboxEventLoop(MailboxEventQueuePtr queue) : queue_(queue) {
  thread_.reset(new std::thread([=]() {
    bool shutdown = false;
    while (!shutdown) {
      auto event_base = queue_->WaitAndPop();
      switch (event_base->GetType()) {
      case MailboxEventType::RecvComm: {
        auto event = std::static_pointer_cast<MailboxEventRecvComm>(event_base);
        int local_shard_id = event->GetLocalShardId();
        int channel_id = event->GetChannelId();
        std::shared_ptr<BinStream> bin_stream = event->GetBinStream();
        if (recv_handlers_.find(channel_id) != recv_handlers_.end()) {
          recv_handlers_.at(channel_id)(local_shard_id, bin_stream);
        } else {
          cached_comm_[channel_id].push_back({local_shard_id, bin_stream});
        }
        break;
      }
      case MailboxEventType::SetRecvHandler: {
        auto event = std::static_pointer_cast<MailboxEventSetRecvHandler>(event_base);
        int channel_id = event->GetChannelId();
        MailboxRecvHandlerType* handler = event->GetHandler();
        recv_handlers_[channel_id] = *handler;

        // Process cached communication
        if (cached_comm_.find(channel_id) != cached_comm_.end()) {
          for (auto& local_shard_id_payload : cached_comm_.at(channel_id)) {
            int local_shard_id = local_shard_id_payload.first;
            std::shared_ptr<BinStream> payload = local_shard_id_payload.second;
            recv_handlers_.at(channel_id)(local_shard_id, payload);
          }
        }
        break;
      }
      case MailboxEventType::SendComm: {
        auto event = std::static_pointer_cast<MailboxEventSendComm>(event_base);
        int process_id = event->GetProcessId();
        int local_shard_id = event->GetLocalShardId();
        int channel_id = event->GetChannelId();
        std::shared_ptr<BinStream> payload = event->GetPayload();
        send_handler_({local_shard_id, process_id}, channel_id, payload.get());
        break;
      }
      case MailboxEventType::Shutdown: {
        shutdown = true;
        break;
      }
      }
    }
  }));
}

MailboxEventLoop::~MailboxEventLoop() {
  auto event = std::make_shared<MailboxEventShutdown>();
  queue_->Push(event);
  thread_->join();
}

void MailboxEventLoop::OnSend(MailboxSendHandlerType handler) { send_handler_ = handler; }

}  // namespace base
}  // namespace husky
