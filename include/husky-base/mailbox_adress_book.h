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

#pragma once

#include <functional>
#include <map>
#include <string>

namespace husky {
namespace base {

class MailboxAddressBook {
 public:
  void AddProcess(int process_id, const std::string& address);
  void RemoveProcess(int process_id);
  std::string GetAddress(int process_id);
  void ForEach(std::function<void(int process, const std::string& addr)> handler);

 protected:
  std::map<int, std::string> address_map_;
};

}  // namespace base
}  // namespace husky
