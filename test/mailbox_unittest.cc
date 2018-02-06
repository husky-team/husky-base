
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
#include "husky-base/mailbox_sender.h"
#include "husky-base/mailbox_recver.h"

// TODO This contains unittests for different components of Mailbox. They should be separated later.

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
  event_loop.OnSend([&](Shard, int channel_id, BinStream*){
      send_triggered = true;
      EXPECT_EQ(0, channel_id);
    });
  mailbox.Send({0, 0}, 0, nullptr);
  std::this_thread::sleep_for(500ms);
  EXPECT_TRUE(send_triggered);
}

TEST_F(TestMailbox, SenderInitDestroy) {
  MailboxSender sender({}, &zmq_context_);
}

TEST_F(TestMailbox, RecverInitDestroy) {
  MailboxRecver recver("inproc://mailbox-recver", "inproc://mailbox-recver", &zmq_context_);
}

TEST_F(TestMailbox, SendRecvSimple) {
  using namespace std::chrono_literals;

  MailboxAddressBook addr_book;
  addr_book.AddProcess(0, "inproc://mailbox-recver");
  MailboxSender sender(addr_book, &zmq_context_);
  MailboxRecver recver("inproc://mailbox-recver", "inproc://mailbox-recver", &zmq_context_);
  MailboxEventLoop event_loop(&zmq_context_);
  event_loop.OnSend(std::bind(&MailboxSender::Send, &sender, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
  Mailbox mailbox(&zmq_context_);
  bool recv_triggered = false;
  mailbox.OnRecv(0, [&](int local_shard_id, BinStream* payload){
      double data;
      *payload >> data;
      EXPECT_EQ(3.14, data);
      EXPECT_EQ(0, local_shard_id);
      recv_triggered = true;
    });
  std::this_thread::sleep_for(500ms);
  BinStream* payload = new BinStream();
  *payload << 3.14;
  sender.Send({0, 0}, 0, payload);
  std::this_thread::sleep_for(500ms);
  EXPECT_TRUE(recv_triggered);
}

}  // namespace
}  // namespace base
}  // namespace husky
