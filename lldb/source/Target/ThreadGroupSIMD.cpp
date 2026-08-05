//===-- ThreadGroupSIMD.cpp -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "lldb/Target/ThreadGroupSIMD.h"

#include "lldb/Target/ABI.h"
#include "lldb/Target/Process.h"

using namespace lldb;
using namespace lldb_private;

void ThreadGroupSIMD::ThreadRegisterWasModified(RegisterInfo *info) {
  ABISP abi_sp = GetABI();
  // See if the register is SIMD scoped. If so, then every thread in SIMD group
  // needs to invalidate this register.
  if (!abi_sp || !abi_sp->RegisterIsSIMDScoped(info))
    return;

  for (auto &thread_wp : m_threads) {
    ThreadSP thread_sp = thread_wp.lock();
    if (thread_sp) {
      lldb::RegisterContextSP reg_ctx_sp = thread_sp->GetRegisterContext();
      if (reg_ctx_sp) {
        // TODO: add an invalidate for just a single register.
        reg_ctx_sp->InvalidateAllRegisters();
      }
    }
  }
}
