#include "husky-base/mailbox_adress_book.h"

namespace husky {
namespace base {

void MailboxAddressBook::AddProcess(int process_id, std::string address) {
  address_map_[process_id] = address;
}

void MailboxAddressBook::RemoveProcess(int process_id) {
  address_map_.erase(process_id);
}

std::string MailboxAddressBook::GetAddress(int process_id) {
  return address_map_.at(process_id);
}

void MailboxAddressBook::ForEach(std::function<void(int process, const std::string& addr)> handler) {
  for (auto& pair : address_map_) {
    handler(pair.first, pair.second);
  }
}

}  // namespace base
}  // namespace husky
