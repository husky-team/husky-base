#include <thread>

#include "zmq.hpp"

#include "husky-base/shard.h"
#include "husky-base/bin_stream.h"

namespace husky {
namespace base {

class MailboxRecver {
 public:
  MailboxRecver(const std::string& addr, zmq::context_t* zmq_context);
  ~MailboxRecver();
 protected:
  std::unique_ptr<std::thread> thread_;
  zmq::context_t* zmq_context_;
  std::string addr_;
};

}  // namespace base
}  // namespace husky
