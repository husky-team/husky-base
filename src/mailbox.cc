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

Mailbox::Mailbox(zmq::context_t* zmq_context) : zmq_context_(zmq_context) {
  socket_.reset(new zmq::socket_t(*zmq_context_, zmq::socket_type::push));
  socket_->connect("inproc://mailbox-event-loop");
}

Mailbox::~Mailbox() { socket_.reset(nullptr); }

void Mailbox::Send(Shard shard, int channel_id, BinStream* payload) {
  zmq_sendmore_int32(socket_.get(), MailboxEventType::SendComm);
  zmq_sendmore_int32(socket_.get(), shard.GetProcessId());
  zmq_sendmore_int32(socket_.get(), shard.GetLocalShardId());
  zmq_sendmore_int32(socket_.get(), channel_id);
  zmq_send_int64(socket_.get(), reinterpret_cast<uint64_t>(payload));
}

void Mailbox::OnRecv(int channel_id, MailboxRecvHandlerType handler) {
  zmq_sendmore_int32(socket_.get(), MailboxEventType::SetRecvHandler);
  zmq_sendmore_int32(socket_.get(), channel_id);
  zmq_send_int64(socket_.get(), reinterpret_cast<uint64_t>(new MailboxRecvHandlerType(handler)));
}

}  // namespace base
}  // namespace husky
