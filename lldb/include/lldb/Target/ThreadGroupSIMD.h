//===-- ThreadGroupSIMD.h -------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_TARGET_THREADGROUPSIMD_H
#define LLDB_TARGET_THREADGROUPSIMD_H

#include "lldb/Target/ThreadGroup.h"

namespace lldb_private {

///-----------------------------------------------------------------------------
/// A group of threads that can be coordinated and acted upon as a group.
///
/// LLDB needs to be able to coordinate the execution or synchronization of
/// threads.  This class is used to represent a group of threads that can be
/// coordinated and acted upon as a group.  This class can be used to represent
/// a group of threads that are part of a SIMD vector, or a group of threads
/// the user has created.  The type of group is represented by the
/// \a ThreadGroup::Type enum.
///-----------------------------------------------------------------------------
class ThreadGroupSIMD : public ThreadGroup {
public:
  ThreadGroupSIMD(lldb::ProcessSP process_sp, lldb::user_id_t uid)
      : ThreadGroup(process_sp, uid) {}

  virtual ~ThreadGroupSIMD() = default;

  /// When a register was modified, we might need to invalidate this same
  /// register in other threads in the SIMD group if the register is SIMD
  /// scoped.
  void ThreadRegisterWasModified(RegisterInfo *info) override;
};

} // namespace lldb_private

#endif // LLDB_TARGET_THREADGROUPSIMD_H
