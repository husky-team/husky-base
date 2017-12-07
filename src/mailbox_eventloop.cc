#include "husky-base/mailbox_eventloop.h"
#include "husky-base/mailbox_constants.h"
#include "husky-base/zmq_helpers.h"

namespace husky {
namespace base {

MailboxEventLoop::MailboxEventLoop(zmq::context_t* zmq_context) : zmq_context_(zmq_context) {
  thread_.reset(new std::thread([=](){
      zmq::socket_t event_loop_socket(*zmq_context_, zmq::socket_type::pull);
      event_loop_socket.bind("inproc://mailbox-event-loop");
      bool shutdown = false;
      while (!shutdown) {
        int type = zmq_recv_int32(&event_loop_socket);
        switch (type) {
          case MAILBOX_EVENT_RECV_COMM: {
            int channel_id = zmq_recv_int32(&event_loop_socket);
            int progress = zmq_recv_int32(&event_loop_socket);
            BinStream* bin_stream = reinterpret_cast<BinStream*>(zmq_recv_int64(&event_loop_socket));
            recv_available_handlers_.at(channel_id)(progress, bin_stream);
            break;
          } case MAILBOX_EVENT_RECV_COMM_COMPLETE: {
            int channel_id = zmq_recv_int32(&event_loop_socket);
            int progress = zmq_recv_int32(&event_loop_socket);
            recv_complete_handlers_.at(channel_id)(progress);
            break;
          } case MAILBOX_EVENT_SEND_COMM: {
            int process_id = zmq_recv_int32(&event_loop_socket);
            int local_shard_id = zmq_recv_int32(&event_loop_socket);
            int channel_id = zmq_recv_int32(&event_loop_socket);
            int progress = zmq_recv_int32(&event_loop_socket);
            BinStream* bin_stream = reinterpret_cast<BinStream*>(zmq_recv_int64(&event_loop_socket));
            send_handler_({process_id, local_shard_id}, channel_id, progress, bin_stream);
            break;
          } case MAILBOX_EVENT_SEND_COMM_COMPLETE: {
            int channel_id = zmq_recv_int32(&event_loop_socket);
            int progress = zmq_recv_int32(&event_loop_socket);
            send_complete_handler_(channel_id, progress);
            break;
          } case MAILBOX_EVENT_SET_RECV_COMM_HANDLER: {
            int channel_id = zmq_recv_int32(&event_loop_socket);
            RecvAvailableHandlerType* handler = reinterpret_cast<RecvAvailableHandlerType*>(zmq_recv_int64(&event_loop_socket));
            recv_available_handlers_[channel_id] = *handler;
            break;
          } case MAILBOX_EVENT_SET_RECV_COMM_COMPLETE_HANDLER: {
            int channel_id = zmq_recv_int32(&event_loop_socket);
            RecvCompleteHandlerType* handler = reinterpret_cast<RecvCompleteHandlerType*>(zmq_recv_int64(&event_loop_socket));
            recv_complete_handlers_[channel_id] = *handler;
            break;
          } case MAILBOX_EVENT_SHUTDOWN: {
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
  zmq_send_int32(&shutdown_sock, MAILBOX_EVENT_SHUTDOWN);
  thread_->join();
}

void MailboxEventLoop::OnSend(SendHandlerType handler) {
  send_handler_ = handler;
}
void MailboxEventLoop::OnSendComplete(SendCompleteHandlerType handler) {
  send_complete_handler_ = handler;
}

}  // namespace base
}  // namespace husky
