//===- lib/MC/RGB9ObjectWriter.cpp - RGB9 File Writer ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements RGB9 object file writer information.
//
//===----------------------------------------------------------------------===//

#include "llvm/BinaryFormat/RGB9.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCRGB9ObjectWriter.h"
#include "llvm/MC/MCValue.h"

using namespace llvm;

#define DEBUG_TYPE "mc"

namespace {

class RGB9ObjectWriter : public MCObjectWriter {
  /// The target specific RGB9 writer instance.
  std::unique_ptr<MCRGB9ObjectTargetWriter> TargetObjectWriter;

  raw_pwrite_stream *OS = nullptr;

public:
  RGB9ObjectWriter(std::unique_ptr<MCRGB9ObjectTargetWriter> MOTW,
                   raw_pwrite_stream &OS_)
      : TargetObjectWriter(std::move(MOTW)), OS(&OS_) {}

private:
  void reset() override {
    MCObjectWriter::reset();
  }

  // MCObjectWriter API
  void executePostLayoutBinding(MCAssembler &Asm,
                                const MCAsmLayout &Layout) override;
  void recordRelocation(MCAssembler &Asm, const MCAsmLayout &Layout,
                        const MCFragment *Fragment,
                        const MCFixup &Fixup, MCValue Target,
                        uint64_t &FixedValue) override;
  uint64_t writeObject(MCAssembler &Asm, const MCAsmLayout &Layout) override;
};

} // end anonymous namespace

void RGB9ObjectWriter::executePostLayoutBinding(MCAssembler &Asm,
                                                const MCAsmLayout &Layout) {
  llvm_unreachable("Not yet implemented for RGB9");
}

void RGB9ObjectWriter::recordRelocation(MCAssembler &Asm,
                                        const MCAsmLayout &Layout,
                                        const MCFragment *Fragment,
                                        const MCFixup &Fixup, MCValue Target,
                                        uint64_t &FixedValue) {
  llvm_unreachable("Not yet implemented for RGB9");
}

uint64_t RGB9ObjectWriter::writeObject(MCAssembler &Asm,
                                       const MCAsmLayout &Layout) {
  llvm_unreachable("Not yet implemented for RGB9");
}

std::unique_ptr<MCObjectWriter>
llvm::createRGB9ObjectWriter(std::unique_ptr<MCRGB9ObjectTargetWriter> MOTW,
                             raw_pwrite_stream &OS) {
  return std::make_unique<RGB9ObjectWriter>(std::move(MOTW), OS);
}
