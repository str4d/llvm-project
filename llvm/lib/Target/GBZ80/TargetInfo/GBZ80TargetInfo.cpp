//===-- GBZ80TargetInfo.cpp - GBZ80 Target Implementation ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
namespace llvm {
Target &getTheGBZ80Target() {
  static Target TheGBZ80Target;
  return TheGBZ80Target;
}
}

extern "C" void LLVMInitializeGBZ80TargetInfo() {
  llvm::RegisterTarget<llvm::Triple::gbz80> X(llvm::getTheGBZ80Target(), "gbz80",
                                            "Game Boy Z80 (LR35902)", "GBZ80");
}

