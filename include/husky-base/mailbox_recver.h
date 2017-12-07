#include "zmq.hpp"

#include "husky-base/shard.h"
#include "husky-base/bin_stream.h"

namespace husky {
namespace base {

class MailboxRecver {
 public:
  MailboxRecver(zmq::context_t* zmq_context);
  ~MailboxRecver();
};

}  // namespace base
}  // namespace husky
