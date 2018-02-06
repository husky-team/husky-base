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

#include "husky-base/mailbox_eventloop.h"

#include "husky-base/zmq_helpers.h"

namespace husky {
namespace base {

MailboxEventLoop::MailboxEventLoop(zmq::context_t* zmq_context) : zmq_context_(zmq_context) {
  thread_.reset(new std::thread([=]() {
    zmq::socket_t event_loop_socket(*zmq_context_, zmq::socket_type::pull);
    event_loop_socket.bind("inproc://mailbox-event-loop");
    bool shutdown = false;
    while (!shutdown) {
      int type = zmq_recv_int32(&event_loop_socket);
      switch (type) {
      case MailboxEventType::RecvComm: {
        int local_shard_id = zmq_recv_int32(&event_loop_socket);
        int channel_id = zmq_recv_int32(&event_loop_socket);
        BinStream* bin_stream = reinterpret_cast<BinStream*>(zmq_recv_int64(&event_loop_socket));
        recv_handlers_.at(channel_id)(local_shard_id, bin_stream);
        break;
      }
      case MailboxEventType::SendComm: {
        int process_id = zmq_recv_int32(&event_loop_socket);
        int local_shard_id = zmq_recv_int32(&event_loop_socket);
        int channel_id = zmq_recv_int32(&event_loop_socket);
        BinStream* payload = reinterpret_cast<BinStream*>(zmq_recv_int64(&event_loop_socket));
        send_handler_({process_id, local_shard_id}, channel_id, payload);
        break;
      }
      case MailboxEventType::SetRecvHandler: {
        int channel_id = zmq_recv_int32(&event_loop_socket);
        MailboxRecvHandlerType* handler = reinterpret_cast<MailboxRecvHandlerType*>(zmq_recv_int64(&event_loop_socket));
        recv_handlers_[channel_id] = *handler;
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
  zmq::socket_t shutdown_sock(*zmq_context_, zmq::socket_type::push);
  shutdown_sock.connect("inproc://mailbox-event-loop");
  zmq_send_int32(&shutdown_sock, MailboxEventType::Shutdown);
  thread_->join();
}

void MailboxEventLoop::OnSend(MailboxSendHandlerType handler) { send_handler_ = handler; }

}  // namespace base
}  // namespace husky
