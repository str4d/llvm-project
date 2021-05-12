//===-- GBZ80MCAsmInfo.h - GBZ80 asm properties ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the GBZ80MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_GBZ80_ASM_INFO_H
#define LLVM_GBZ80_ASM_INFO_H

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {

class Triple;

/// Specifies the format of GBZ80 assembly files.
class GBZ80MCAsmInfo : public MCAsmInfo {
public:
  explicit GBZ80MCAsmInfo(const Triple &TT);
};

} // end namespace llvm

#endif // LLVM_GBZ80_ASM_INFO_H
