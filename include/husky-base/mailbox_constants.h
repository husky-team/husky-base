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

// Mailbox event types
const int MAILBOX_EVENT_RECV_COMM = 1;
const int MAILBOX_EVENT_RECV_COMM_COMPLETE = 2;
const int MAILBOX_EVENT_SET_RECV_COMM_HANDLER = 3;
const int MAILBOX_EVENT_SET_RECV_COMM_COMPLETE_HANDLER = 4;
const int MAILBOX_EVENT_SEND_COMM = 5;
const int MAILBOX_EVENT_SEND_COMM_COMPLETE = 6;
const int MAILBOX_EVENT_SHUTDOWN = 7;

// Mailbox communication types
const int MAILBOX_COMM = 8;
const int MAILBOX_COMM_COMPLETE = 9;
