
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

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

#include "husky-base/mailbox.h"
#include "husky-base/mailbox_eventloop.h"

namespace husky {
namespace base {
namespace {

class TestMailbox : public testing::Test {
 public:
  TestMailbox() {}
  ~TestMailbox() {}

 protected:
  void SetUp() {}
  void TearDown() {}
  zmq::context_t zmq_context_;
};

TEST_F(TestMailbox, EventLoopInitDestroy) {
  MailboxEventLoop event_loop(&zmq_context_);
}

TEST_F(TestMailbox, SendTriggerEventLoop) {
  using namespace std::chrono_literals;

  zmq::context_t zmq_context;
  Mailbox mailbox(&zmq_context_);
  MailboxEventLoop event_loop(&zmq_context_);
  bool send_triggered = false;
  event_loop.OnSend([&](Shard, int channel_id, int progress, BinStream*){
      send_triggered = true;
      EXPECT_EQ(0, channel_id);
      EXPECT_EQ(0, progress);
    });
  mailbox.Send({0, 0}, 0, 0, nullptr);
  std::this_thread::sleep_for(500ms);
  EXPECT_TRUE(send_triggered);
}

TEST_F(TestMailbox, SendCompleteTriggerEventLoop) {
  using namespace std::chrono_literals;

  zmq::context_t zmq_context;
  Mailbox mailbox(&zmq_context_);
  MailboxEventLoop event_loop(&zmq_context_);
  bool send_complete_triggered = false;
  event_loop.OnSendComplete([&](int channel_id, int progress){
      send_complete_triggered = true;
      EXPECT_EQ(0, channel_id);
      EXPECT_EQ(0, progress);
    });
  mailbox.SendComplete(0, 0);
  std::this_thread::sleep_for(500ms);
  EXPECT_TRUE(send_complete_triggered);
}

}  // namespace
}  // namespace base
}  // namespace husky
