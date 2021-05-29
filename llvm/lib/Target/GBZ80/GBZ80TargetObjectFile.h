//===-- GBZ80TargetObjectFile.h - GBZ80 Object Info -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_GBZ80_TARGET_OBJECT_FILE_H
#define LLVM_GBZ80_TARGET_OBJECT_FILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

/// Lowering for a GBZ80 RGB9 object file.
class GBZ80TargetObjectFile : public TargetLoweringObjectFileRGB9 {
  typedef TargetLoweringObjectFileRGB9 Base;

public:
  void Initialize(MCContext &ctx, const TargetMachine &TM) override;
};

} // end namespace llvm

#endif // LLVM_GBZ80_TARGET_OBJECT_FILE_H
