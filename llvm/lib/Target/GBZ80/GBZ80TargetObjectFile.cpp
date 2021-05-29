//===-- GBZ80TargetObjectFile.cpp - GBZ80 Object Files ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "GBZ80TargetObjectFile.h"

#include "llvm/BinaryFormat/ELF.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionRGB9.h"

#include "GBZ80.h"

namespace llvm {

void GBZ80TargetObjectFile::Initialize(MCContext &Ctx, const TargetMachine &TM) {
  Base::Initialize(Ctx, TM);
}

} // end of namespace llvm

