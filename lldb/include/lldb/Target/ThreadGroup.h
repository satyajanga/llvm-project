//===-- ThreadGroup.h -----------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_TARGET_THREADGROUP_H
#define LLDB_TARGET_THREADGROUP_H

#include <mutex>
#include <stdint.h>
#include <vector>

#include "lldb/Utility/UserID.h"
#include "lldb/lldb-forward.h"
namespace lldb_private {

struct RegisterInfo;

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
class ThreadGroup : public UserID {
public:
  ThreadGroup(lldb::ProcessSP process_sp, lldb::user_id_t uid)
      : UserID(uid), m_process_wp(process_sp) {}

  virtual ~ThreadGroup() = default;

  uint32_t GetSize() const;

  void AddThread(const lldb::ThreadSP &thread_sp);

  // Note that "idx" is not the same as the "thread_index". It is a zero based
  // index to accessing the current threads, whereas "thread_index" is a unique
  // index assigned
  lldb::ThreadSP GetThreadAtIndex(uint32_t idx) const;

  std::recursive_mutex &GetMutex() const { return m_mutex; }

  /// Notifify this thread group that one of the thread has been resumed.
  ///
  /// Subclasses can override this method to do something when a thread is
  /// resumed, like clear SIMD scoped registers that apply to all threads in
  /// the group.
  virtual void ThreadWasResumed(Thread &thread) {}

  /// Notifify this thread group that one of the registers has been modified.
  ///
  /// Subclasses can override this method to do something when a register is
  /// modified, like invalidate a SIMD scoped register in all other threads.
  virtual void ThreadRegisterWasModified(RegisterInfo *info) {}

protected:
  /// Get the ABI plug-in from the process that owns this thread group.
  lldb::ABISP GetABI() const;

  // Only hold a weak reference to the thread, so that we don't keep the thread
  // objects alive longer than we need to.
  lldb::ProcessWP m_process_wp;
  typedef std::vector<lldb::ThreadWP> collection;
  lldb::tid_t m_user_id;
  collection m_threads;
  mutable std::recursive_mutex m_mutex;
};

} // namespace lldb_private

#endif // LLDB_TARGET_THREADGROUP_H
