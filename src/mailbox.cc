#include "husky-base/mailbox.h"
#include "husky-base/mailbox_constants.h"
#include "husky-base/zmq_helpers.h"

namespace husky {
namespace base {

Mailbox::Mailbox(zmq::context_t* zmq_context) : zmq_context_(zmq_context) {
  socket_.reset(new zmq::socket_t(*zmq_context_, zmq::socket_type::push));
  socket_->connect("inproc://mailbox-event-loop");
}

Mailbox::~Mailbox() {
  socket_.reset(nullptr);
}

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
