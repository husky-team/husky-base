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
#include "husky-base/mailbox_constants.h"
#include "husky-base/zmq_helpers.h"

namespace husky {
namespace base {

Mailbox::Mailbox(zmq::context_t* zmq_context) : zmq_context_(zmq_context) {
  socket_.reset(new zmq::socket_t(*zmq_context_, zmq::socket_type::push));
  socket_->connect("inproc://mailbox-event-loop");
}

Mailbox::~Mailbox() { socket_.reset(nullptr); }

void Mailbox::Send(Shard shard, int channel_id, int progress, BinStream* bin_stream) {
  zmq_sendmore_int32(socket_.get(), MAILBOX_EVENT_SEND_COMM);
  zmq_sendmore_int32(socket_.get(), shard.GetProcessId());
  zmq_sendmore_int32(socket_.get(), shard.GetLocalShardId());
  zmq_sendmore_int32(socket_.get(), channel_id);
  zmq_sendmore_int32(socket_.get(), progress);
  zmq_send_int64(socket_.get(), reinterpret_cast<uint64_t>(bin_stream));
}

void Mailbox::SendComplete(int channel_id, int progress) {
  zmq_sendmore_int32(socket_.get(), MAILBOX_EVENT_SEND_COMM_COMPLETE);
  zmq_sendmore_int32(socket_.get(), channel_id);
  zmq_send_int32(socket_.get(), progress);
}

void Mailbox::SetRecvAvailableHandler(int channel_id, RecvAvailableHandlerType handler) {
  zmq_sendmore_int32(socket_.get(), MAILBOX_EVENT_SET_RECV_COMM_HANDLER);
  zmq_sendmore_int32(socket_.get(), channel_id);
  zmq_send_int64(socket_.get(), reinterpret_cast<uint64_t>(new RecvAvailableHandlerType(handler)));
}

void Mailbox::SetRecvCompleteHandler(int channel_id, RecvCompleteHandlerType handler) {
  zmq_sendmore_int32(socket_.get(), MAILBOX_EVENT_SET_RECV_COMM_COMPLETE_HANDLER);
  zmq_sendmore_int32(socket_.get(), channel_id);
  zmq_send_int64(socket_.get(), reinterpret_cast<uint64_t>(new RecvCompleteHandlerType(handler)));
}

}  // namespace base
}  // namespace husky
