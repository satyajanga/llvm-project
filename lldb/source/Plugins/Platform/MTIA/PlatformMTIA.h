
#include <memory>
#include <string>

#include "lldb/Target/RemoteAwarePlatform.h"

namespace lldb_private {
namespace platform_mtia {

class PlatformMTIA : public lldb_private::RemoteAwarePlatform {
public:
  PlatformMTIA(bool is_host);
  ~PlatformMTIA() override = default;

  // lldb_private::PluginInterface functions
  static lldb::PlatformSP CreateInstance(bool force, const ArchSpec *arch);

  static llvm::StringRef GetPluginNameStatic(bool is_host) {
    return is_host ? Platform::GetHostPlatformName() : "remote-mtia";
  }

  static llvm::StringRef GetPluginDescriptionStatic(bool is_host);

  llvm::StringRef GetPluginName() override {
    return GetPluginNameStatic(IsHost());
  }

  // lldb_private::Platform functions
  llvm::StringRef GetDescription() override {
    return GetPluginDescriptionStatic(IsHost());
  }

  static void Initialize();

  static void Terminate();

  static void DebuggerInitialize(lldb_private::Debugger &debugger);

  std::vector<ArchSpec>
  GetSupportedArchitectures(const ArchSpec &process_host_arch) override;

  Status ConnectRemote(Args &args) override;

  lldb::ProcessSP Attach(lldb_private::ProcessAttachInfo &attach_info,
                         lldb_private::Debugger &debugger,
                         lldb_private::Target *target, // Can be nullptr, if
                                                       // nullptr create a new
                                                       // target, else use
                                                       // existing one
                         lldb_private::Status &error) override;

  void CalculateTrapHandlerSymbolNames() override;

private:
  std::string m_device_id;
};

} // namespace platform_mtia
} // namespace lldb_private
