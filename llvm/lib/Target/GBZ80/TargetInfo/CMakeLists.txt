include_directories( ${CMAKE_CURRENT_BINARY_DIR}/..
                     ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_component_library(LLVMGBZ80Info
  GBZ80TargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  GBZ80
)

add_dependencies(LLVMGBZ80Info GBZ80CommonTableGen)

