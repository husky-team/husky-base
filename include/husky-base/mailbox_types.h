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

#include "husky-base/shard.h"

namespace husky {
namespace base {

// Mailbox event types
enum MailboxEventType : int32_t { RecvComm = 1, SetRecvHandler = 2, SendComm = 3, Shutdown = 4 };

// Typedefs for send/recv handler
typedef std::function<void(Shard shard, int channel_id, BinStream* payload)> MailboxSendHandlerType;
typedef std::function<void(int local_shard_id, BinStream* payload)> MailboxRecvHandlerType;

}  // namespace base
}  // namespace husky
