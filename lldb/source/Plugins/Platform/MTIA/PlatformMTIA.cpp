

#include "PlatformMTIA.h"

#include "Plugins/Platform/gdb-server/PlatformRemoteGDBServer.h"
#include "lldb/Core/Debugger.h"
#include "lldb/Core/Module.h"
#include "lldb/Core/PluginManager.h"
#include "lldb/Target/Process.h"
#include "lldb/Utility/LLDBLog.h"
#include "lldb/Utility/Log.h"

using namespace lldb;
using namespace lldb_private;
using namespace lldb_private::platform_mtia;

LLDB_PLUGIN_DEFINE(PlatformMTIA)

PlatformSP PlatformMTIA::CreateInstance(bool force, const ArchSpec *arch) {

  Log *log = GetLog(LLDBLog::Platform);
  LLDB_LOG(log, "force = {0}, arch=({1}, {2})", force,
           arch ? arch->GetArchitectureName() : "<null>",
           arch ? arch->GetTriple().getTriple() : "<null>");

  bool create = force;
  if (!create && arch && arch->IsValid()) {
    const llvm::Triple &triple = arch->GetTriple();
    if (triple.getOS() != llvm::Triple::UnknownOS &&
        triple.getEnvironment() == llvm::Triple::MTIA) {
      create = true;
    }
  }

  LLDB_LOG(log, "create = {0}", create);
  if (create) {
    return PlatformSP(new PlatformMTIA(false));
  }
  return PlatformSP();
}

PlatformMTIA::PlatformMTIA(bool is_host)
    : lldb_private::RemoteAwarePlatform(is_host) {}

llvm::StringRef PlatformMTIA::GetPluginDescriptionStatic(bool is_host) {
  if (is_host)
    return "Local MTIA user platform plug-in.";
  return "Remote MTIA user platform plug-in.";
}

uint32_t g_initialize_count = 0;

void PlatformMTIA::Initialize() {
  if (g_initialize_count++ == 0) {
    PluginManager::RegisterPlugin(
        PlatformMTIA::GetPluginNameStatic(false),
        PlatformMTIA::GetPluginDescriptionStatic(false),
        PlatformMTIA::CreateInstance, PlatformMTIA::DebuggerInitialize);
  }
}

void PlatformMTIA::Terminate() {
  if (g_initialize_count > 0) {
    if (--g_initialize_count == 0) {
      PluginManager::UnregisterPlugin(PlatformMTIA::CreateInstance);
    }
  }

}

void PlatformMTIA::DebuggerInitialize(Debugger &debugger) {}

Status PlatformMTIA::ConnectRemote(Args &args) {
  m_device_id.clear();

  Status error;
  if (IsHost()) {
    error = Status::FromErrorStringWithFormatv(
        "can't connect to the host platform '{0}', always connected",
        GetPluginName());
  } else {
    if (!m_remote_platform_sp)
      m_remote_platform_sp =
          platform_gdb_server::PlatformRemoteGDBServer::CreateInstance(
              /*force=*/true, nullptr);

    if (m_remote_platform_sp && error.Success())
      error = m_remote_platform_sp->ConnectRemote(args);
    else
      error = Status::FromErrorString(
          "failed to create a 'remote-gdb-server' platform");

    if (error.Fail())
      m_remote_platform_sp.reset();
  }
  return error;
}

lldb::ProcessSP
PlatformMTIA::Attach(lldb_private::ProcessAttachInfo &attach_info,
                     lldb_private::Debugger &debugger,
                     lldb_private::Target *target,
                     lldb_private::Status &error) {
  lldb::ProcessSP process_sp;
  Log *log = GetLog(LLDBLog::Platform);
  if (IsHost()) {
    if (target == nullptr) {
      TargetSP new_target_sp;
      error = debugger.GetTargetList().CreateTarget(
          debugger, "", "", eLoadDependentsNo, nullptr, new_target_sp);
      target = new_target_sp.get();
      LLDB_LOGF(log, "PlatformPOSIX::%s created new target", __FUNCTION__);
    } else {
      error.Clear();
      LLDB_LOGF(log, "PlatformPOSIX::%s target already existed, setting target",
                __FUNCTION__);
    }

    if (target && error.Success()) {
      if (log) {
        ModuleSP exe_module_sp = target->GetExecutableModule();
        LLDB_LOGF(log, "PlatformPOSIX::%s set selected target to %p %s",
                  __FUNCTION__, (void *)target,
                  exe_module_sp ? exe_module_sp->GetFileSpec().GetPath().c_str()
                                : "<null>");
      }

      process_sp =
          target->CreateProcess(attach_info.GetListenerForProcess(debugger),
                                "gdb-remote", nullptr, true);
    }
  } else {
    if (m_remote_platform_sp)
      process_sp =
          m_remote_platform_sp->Attach(attach_info, debugger, target, error);
    else
      error =
          Status::FromErrorString("the platform is not currently connected");
  }
  return process_sp;
}

void PlatformMTIA::CalculateTrapHandlerSymbolNames() {}
