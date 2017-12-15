#include <functional>
#include <thread>
#include <map>

#include "zmq.hpp"

#include "husky-base/shard.h"
#include "husky-base/bin_stream.h"
#include "husky-base/mailbox_adress_book.h"

namespace husky {
namespace base {

class MailboxEventLoop {
 public:
  typedef std::function<void(int progress, BinStream*)> RecvAvailableHandlerType;
  typedef std::function<void(int progress)> RecvCompleteHandlerType;
  typedef std::function<void(Shard shard, int channel_id, int progress, BinStream*)> SendHandlerType;
  typedef std::function<void(int channel_id, int progress)> SendCompleteHandlerType;

  explicit MailboxEventLoop(zmq::context_t* zmq_context);
  ~MailboxEventLoop();

  void OnSend(SendHandlerType handler);
  void OnSendComplete(SendCompleteHandlerType handler);
 protected:
  std::unique_ptr<std::thread> thread_;
  zmq::context_t* zmq_context_;
  std::map<int, RecvAvailableHandlerType> recv_available_handlers_;
  std::map<int, RecvCompleteHandlerType> recv_complete_handlers_;
  SendHandlerType send_handler_;
  SendCompleteHandlerType send_complete_handler_;
};

}  // namespace base
}  // namespace husky
