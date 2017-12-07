#include <map>
#include <mutex>
#include <string>

// Thread-safe class for keeping the process-id-to-address mapping
class MailboxAddressBook {
 public:
  void AddProcess(int process_id, std::string address);
  void RemoveProcess(int process_id);
  std::string GetAddress(int process_id);
 protected: 
  std::mutex mutex_;
  std::map<int, std::string> address_map_;
};
