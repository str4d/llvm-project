add_llvm_component_library(LLVMGBZ80Desc
  GBZ80AsmBackend.cpp
  GBZ80InstPrinter.cpp
  GBZ80MCAsmInfo.cpp
  GBZ80MCCodeEmitter.cpp
  GBZ80MCExpr.cpp
  GBZ80MCTargetDesc.cpp
  GBZ80RGB9ObjectWriter.cpp
  GBZ80TargetStreamer.cpp

  LINK_COMPONENTS
  MC
  GBZ80Info
  Support

  ADD_TO_COMPONENT
  GBZ80
)

add_dependencies(LLVMGBZ80Desc GBZ80CommonTableGen)

