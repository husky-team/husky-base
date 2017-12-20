// Copyright 2017 Husky Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "husky-base/mailbox_adress_book.h"

#include <string>

namespace husky {
namespace base {

void MailboxAddressBook::AddProcess(int process_id, const std::string& address) { address_map_[process_id] = address; }

void MailboxAddressBook::RemoveProcess(int process_id) { address_map_.erase(process_id); }

std::string MailboxAddressBook::GetAddress(int process_id) { return address_map_.at(process_id); }

void MailboxAddressBook::ForEach(std::function<void(int process, const std::string& addr)> handler) {
  for (auto& pair : address_map_) {
    handler(pair.first, pair.second);
  }
}

}  // namespace base
}  // namespace husky
