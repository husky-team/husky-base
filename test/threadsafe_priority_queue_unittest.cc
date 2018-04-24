
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
#include <cstdlib>
#include <vector>

#include "gtest/gtest.h"

#include "husky-base/threadsafe_priority_queue.h"
#include "husky-base/mailbox_types.h"

// TODO This contains unittests for different components of Mailbox. They should be separated later.

namespace husky {
namespace base {
namespace {

class TestEventQueue : public testing::Test {
 public:
  TestEventQueue() {}
  ~TestEventQueue() {}

 protected:
  void SetUp() {}
  void TearDown() {}
};

TEST_F(TestEventQueue, InitDestroy) {
  ThreadsafePriorityQueue<std::shared_ptr<MailboxEventBase>> queue;
}

TEST_F(TestEventQueue, Functional) {
  ThreadsafePriorityQueue<std::shared_ptr<MailboxEventBase>> queue;
  std::vector<std::shared_ptr<MailboxEventBase>> vec;
  std::vector<std::shared_ptr<MailboxEventBase>> test_vec;
  int n = n;
  for(int i = 0; i < n; ++i) {
    MailboxEventType type = static_cast<MailboxEventType>(rand() % 4 + 1);
    vec.push_back(std::make_shared<MailboxEventBase>(type, rand() % n));
  }
  for(auto ptr: vec) {
    queue.Push(ptr);
  }
  for(int i = 0; i < n; ++i) {
    auto ptr = queue.WaitAndPop();
    test_vec.push_back(ptr);
  }
  std::stable_sort(vec.begin(), vec.end(), [](const std::shared_ptr<MailboxEventBase>& lhs, const std::shared_ptr<MailboxEventBase>& rhs) {
      return lhs->GetPriority() < rhs->GetPriority();
      });
  for(int i = 0; i < n; ++i) {
    EXPECT_EQ(vec.at(i)->GetPriority(), test_vec.at(i)->GetPriority());
    EXPECT_EQ(vec.at(i)->GetType(), test_vec.at(i)->GetType());
  }
}

}  // namespace
}  // namespace base
}  // namespace husky
