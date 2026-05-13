//===-- NativeThreadProtocol.cpp ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "lldb/Host/common/NativeThreadProtocol.h"

#include "lldb/Host/common/NativeProcessProtocol.h"
#include "lldb/Host/common/NativeRegisterContext.h"

using namespace lldb;
using namespace lldb_private;

NativeThreadProtocol::NativeThreadProtocol(NativeProcessProtocol &process,
                                           lldb::tid_t tid)
    : m_process(process), m_tid(tid) {}


NativeThreadProtocol::NativeThreadProtocol(NativeProcessProtocol &process,
                                           lldb::tid_t tid,
                                           lldb::tid_t lane_id)
    : m_process(process), m_tid(tid), m_lane_id(lane_id) {}

bool NativeThreadProtocol::HasValidStopReason() {
  ThreadStopInfo stop_info;
  std::string stop_description;
  if (!GetStopReason(stop_info, stop_description))
    return false;

  return stop_info.reason != lldb::eStopReasonInvalid &&
         stop_info.reason != lldb::eStopReasonNone;
}
