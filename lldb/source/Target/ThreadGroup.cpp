//===-- ThreadGroup.cpp ---------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "lldb/Target/ThreadGroup.h"
#include "lldb/Target/Process.h"

using namespace lldb;
using namespace lldb_private;

uint32_t ThreadGroup::GetSize() const {
  std::lock_guard<std::recursive_mutex> guard(GetMutex());
  return m_threads.size();
}

void ThreadGroup::AddThread(const ThreadSP &thread_sp) {
  std::lock_guard<std::recursive_mutex> guard(GetMutex());
  m_threads.push_back(thread_sp);
}

ThreadSP ThreadGroup::GetThreadAtIndex(uint32_t idx) const {
  std::lock_guard<std::recursive_mutex> guard(GetMutex());
  ThreadSP thread_sp;
  if (idx < m_threads.size())
    thread_sp = m_threads[idx].lock();
  return thread_sp;
}

lldb::ABISP ThreadGroup::GetABI() const {
  ProcessSP process_sp = m_process_wp.lock();
  if (process_sp)
    return process_sp->GetABI();
  return nullptr;
}
