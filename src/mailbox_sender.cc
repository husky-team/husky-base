#include "husky-base/mailbox_sender.h"
#include "husky-base/mailbox_constants.h"
#include "husky-base/zmq_helpers.h"

namespace husky {
namespace base {

MailboxSender::MailboxSender(const MailboxAddressBook& addr_book, zmq::context_t* zmq_context) : zmq_context_(zmq_context), addr_book_(addr_book) {
  addr_book_.ForEach([=](int process_id, const std::string& addr){
      senders_[process_id].reset(new zmq::socket_t(*zmq_context_, zmq::socket_type::push));
      senders_[process_id]->connect(addr);
    });
}

void MailboxSender::Send(Shard shard, int channel_id, int progress, BinStream* bin_stream) {
  auto* sender = senders_.at(shard.GetProcessId()).get();
  zmq_sendmore_int32(sender, MAILBOX_COMM);
  zmq_sendmore_int32(sender, shard.GetLocalShardId());
  zmq_sendmore_int32(sender, channel_id);
  zmq_sendmore_int32(sender, progress);
  zmq_send_binstream(sender, *bin_stream);
  delete bin_stream;
}

void MailboxSender::SendComplete(int channel_id, int progress) {
  // TODO Could also let the upper layers decide how many processes it wants to notify
  for (auto& proc_sender : senders_) {
    auto* sender = proc_sender.second.get();
    zmq_sendmore_int32(sender, MAILBOX_COMM);
    zmq_sendmore_int32(sender, channel_id);
    zmq_send_int32(sender, progress);
  }
}

}  // namespace base
}  // namespace husky
