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
