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

#include "husky-base/mailbox_sender.h"

#include <string>

#include "husky-base/mailbox_types.h"
#include "husky-base/zmq_helpers.h"

namespace husky {
namespace base {

MailboxSender::MailboxSender(zmq::context_t* zmq_context) : zmq_context_(zmq_context) {}

MailboxSender::MailboxSender(const MailboxAddressBook& addr_book, zmq::context_t* zmq_context)
    : zmq_context_(zmq_context), addr_book_(addr_book) {
  addr_book_.ForEach([=](int process_id, const std::string& addr) {
    senders_[process_id].reset(new zmq::socket_t(*zmq_context_, zmq::socket_type::push));
    senders_[process_id]->connect(addr);
  });
}

void MailboxSender::AddNeighbor(int process_id, const std::string& addr) {
  addr_book_.AddProcess(process_id, addr);
  senders_[process_id].reset(new zmq::socket_t(*zmq_context_, zmq::socket_type::push));
  senders_[process_id]->connect(addr);
}

void MailboxSender::RemoveNeighbor(int process_id) {
  addr_book_.RemoveProcess(process_id);
  senders_.erase(process_id);
}

void MailboxSender::Send(Shard shard, int channel_id, BinStream* payload) {
  auto* sender = senders_.at(shard.GetProcessId()).get();
  zmq_sendmore_int32(sender, MailboxEventType::RecvComm);
  zmq_sendmore_int32(sender, shard.GetLocalShardId());
  zmq_sendmore_int32(sender, channel_id);
  zmq_send_binstream(sender, *payload);
}

}  // namespace base
}  // namespace husky
