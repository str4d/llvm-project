//===-- GBZ80TargetStreamer.cpp - GBZ80 Target Streamer Methods ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides GBZ80 specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "GBZ80TargetStreamer.h"

#include "llvm/MC/MCContext.h"

namespace llvm {

GBZ80TargetStreamer::GBZ80TargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

GBZ80TargetAsmStreamer::GBZ80TargetAsmStreamer(MCStreamer &S)
    : GBZ80TargetStreamer(S) {}

void GBZ80TargetStreamer::finish() {

}

GBZ80TargetRGB9Streamer::GBZ80TargetRGB9Streamer(MCStreamer &S)
    : GBZ80TargetStreamer(S) {}

} // end namespace llvm

