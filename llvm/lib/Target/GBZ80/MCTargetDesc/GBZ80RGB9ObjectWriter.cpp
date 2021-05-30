//===-- GBZ80RGB9ObjectWriter.cpp - GBZ80 RGB9 Writer ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/GBZ80MCTargetDesc.h"
#include "llvm/MC/MCRGB9ObjectWriter.h"

using namespace llvm;

namespace {
/// Writes GBZ80 machine code into an RGB9 object file.
class GBZ80RGB9ObjectWriter : public MCRGB9ObjectTargetWriter {
public:
  GBZ80RGB9ObjectWriter();
};
} // end anonymous namespace

GBZ80RGB9ObjectWriter::GBZ80RGB9ObjectWriter()
    : MCRGB9ObjectTargetWriter() {}

std::unique_ptr<MCObjectTargetWriter> llvm::createGBZ80RGB9ObjectWriter() {
  return std::make_unique<GBZ80RGB9ObjectWriter>();
}
