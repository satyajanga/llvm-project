add_lldb_library(lldbPluginPlatformMTIA
  PlatformMTIA.cpp

   LINK_LIBS
    lldbCore
    lldbTarget
    lldbUtility
    lldbPluginPlatformGDB
  )
