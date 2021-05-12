//===-- GBZ80TargetStreamer.h - GBZ80 Target Streamer --------------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_GBZ80_TARGET_STREAMER_H
#define LLVM_GBZ80_TARGET_STREAMER_H

#include "llvm/MC/MCELFStreamer.h"

namespace llvm {
class MCStreamer;

/// A generic GBZ80 target output stream.
class GBZ80TargetStreamer : public MCTargetStreamer {
public:
  explicit GBZ80TargetStreamer(MCStreamer &S);

  void finish() override;
};

/// A target streamer for textual GBZ80 assembly code.
class GBZ80TargetAsmStreamer : public GBZ80TargetStreamer {
public:
  explicit GBZ80TargetAsmStreamer(MCStreamer &S);
};

} // end namespace llvm

#endif // LLVM_GBZ80_TARGET_STREAMER_H
