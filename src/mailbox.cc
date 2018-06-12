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

#include "husky-base/mailbox.h"
#include "husky-base/zmq_helpers.h"

namespace husky {
namespace base {

Mailbox::Mailbox(MailboxEventQueuePtr queue) : queue_(queue) {}

Mailbox::~Mailbox() {}

void Mailbox::Send(Shard shard, int channel_id, BinStream* payload, int priority) {
  auto event = std::make_shared<MailboxEventSendComm>(shard.GetProcessId(), shard.GetLocalShardId(), channel_id,
                                                      std::shared_ptr<BinStream>(payload), priority);
  queue_->Push(event);
}

void Mailbox::OnRecv(int channel_id, MailboxRecvHandlerType handler) {
  auto event = std::make_shared<MailboxEventSetRecvHandler>(channel_id, handler);
  queue_->Push(event);
}

}  // namespace base
}  // namespace husky
