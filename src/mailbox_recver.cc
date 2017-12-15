#include "husky-base/mailbox_recver.h"
#include "husky-base/zmq_helpers.h"
#include "husky-base/mailbox_constants.h"

namespace husky {
namespace base {

MailboxRecver::MailboxRecver(const std::string& addr, zmq::context_t* zmq_context) : addr_(addr), zmq_context_(zmq_context) {
  thread_.reset(new std::thread([=](){
      zmq::socket_t recv_socket(*zmq_context_, zmq::socket_type::pull);
      recv_socket.bind(addr_);

      zmq::socket_t event_socket(*zmq_context_, zmq::socket_type::push);
      event_socket.connect("inproc://mailbox-event-loop");

      bool shutdown = false;
      while(!shutdown) {
        int comm_type = zmq_recv_int32(&recv_socket);
        switch(comm_type) {
          case MAILBOX_COMM : {
            int local_shard_id = zmq_recv_int32(&recv_socket);
            int channel_id = zmq_recv_int32(&recv_socket);
            int progress = zmq_recv_int32(&recv_socket);
            auto* bin_stream = new BinStream(std::move(zmq_recv_binstream(&recv_socket)));
            zmq_sendmore_int32(&event_socket, MAILBOX_EVENT_RECV_COMM);
            zmq_sendmore_int32(&event_socket, local_shard_id);
            zmq_sendmore_int32(&event_socket, channel_id);
            zmq_sendmore_int32(&event_socket, progress);
            zmq_send_int64(&event_socket, reinterpret_cast<uint64_t>(bin_stream));
            break;
          } case MAILBOX_COMM_COMPLETE : {
            int channel_id = zmq_recv_int32(&recv_socket);
            int progress = zmq_recv_int32(&recv_socket);
            zmq_sendmore_int32(&event_socket, MAILBOX_EVENT_RECV_COMM_COMPLETE);
            zmq_sendmore_int32(&event_socket, channel_id);
            zmq_send_int32(&event_socket, progress);
            break;
          } case MAILBOX_EVENT_SHUTDOWN: {
            shutdown = true;
            break;
          }
        }
      }
    }));
}

MailboxRecver::~MailboxRecver() {
  zmq::socket_t shutdown_sock(*zmq_context_, zmq::socket_type::push);
  shutdown_sock.connect(addr_);
  zmq_send_int32(&shutdown_sock, MAILBOX_EVENT_SHUTDOWN);
  thread_->join();
}

}  // namespace base
}  // namespace husky
