include_directories( ${CMAKE_CURRENT_BINARY_DIR}/..
                     ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_component_library(LLVMGBZ80AsmPrinter
  GBZ80InstPrinter.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  GBZ80
  )

add_dependencies(LLVMGBZ80AsmPrinter GBZ80CommonTableGen)