#include "zmq.hpp"

#include "husky-base/shard.h"
#include "husky-base/mailbox_adress_book.h"
#include "husky-base/bin_stream.h"

namespace husky {
namespace base {

class MailboxSender {
 public:
  MailboxSender(const MailboxAddressBook& addr_book, zmq::context_t* zmq_context);

  // This takes over the ownership of bin_stream
  void Send(Shard shard, BinStream* bin_stream);
 protected:
  MailboxAddressBook addr_book_;
};

}  // namespace base
}  // namespace husky
