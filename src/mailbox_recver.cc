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

#include "husky-base/mailbox_recver.h"

#include <sys/socket.h>
#include <string>

#include "husky-base/mailbox_types.h"
#include "husky-base/zmq_helpers.h"

namespace husky {
namespace base {

MailboxRecver::MailboxRecver(const std::string& bind_addr, const std::string& connect_addr, zmq::context_t* zmq_context,
                             MailboxEventQueuePtr queue)
    : bind_addr_(bind_addr), connect_addr_(connect_addr), zmq_context_(zmq_context), queue_(queue) {
  thread_.reset(new std::thread([=]() {
    zmq::socket_t recv_socket(*zmq_context_, zmq::socket_type::pull);
    int sock_fd;
    size_t size = sizeof(sock_fd);
    int flag = 1;
    recv_socket.getsockopt(ZMQ_FD, &sock_fd, &size);
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    recv_socket.bind(bind_addr_);

    bool shutdown = false;
    while (!shutdown) {
      int comm_type = zmq_recv_int32(&recv_socket);
      switch (comm_type) {
      case MailboxEventType::RecvComm: {
        // Relay the event to the event loop
        int local_shard_id = zmq_recv_int32(&recv_socket);
        int channel_id = zmq_recv_int32(&recv_socket);
        auto* bin_stream = new BinStream(std::move(zmq_recv_binstream(&recv_socket)));
        auto event = std::make_shared<MailboxEventRecvComm>(local_shard_id, channel_id, bin_stream);
        queue_->Push(event);
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

MailboxRecver::~MailboxRecver() {
  zmq::socket_t shutdown_sock(*zmq_context_, zmq::socket_type::push);
  shutdown_sock.connect(connect_addr_);
  zmq_send_int32(&shutdown_sock, MailboxEventType::Shutdown);
  thread_->join();
}

}  // namespace base
}  // namespace husky
