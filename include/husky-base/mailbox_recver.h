#include <thread>

#include "zmq.hpp"

#include "husky-base/shard.h"
#include "husky-base/bin_stream.h"

namespace husky {
namespace base {

class MailboxRecver {
 public:
  MailboxRecver(const std::string& bind_addr, const std::string& connect_addr, zmq::context_t* zmq_context);
  ~MailboxRecver();
 protected:
  std::unique_ptr<std::thread> thread_;
  zmq::context_t* zmq_context_;
  std::string bind_addr_;
  std::string connect_addr_;
};

}  // namespace base
}  // namespace husky
