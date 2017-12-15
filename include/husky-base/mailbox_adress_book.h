#pragma once

#include <map>
#include <functional>
#include <string>

namespace husky {
namespace base {

class MailboxAddressBook {
 public:
  void AddProcess(int process_id, std::string address);
  void RemoveProcess(int process_id);
  std::string GetAddress(int process_id);
  void ForEach(std::function<void(int process, const std::string& addr)> handler);
 protected: 
  std::map<int, std::string> address_map_;
};

}  // namespace base
}  // namespace husky
