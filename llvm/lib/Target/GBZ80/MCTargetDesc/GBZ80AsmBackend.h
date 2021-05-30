//===-- GBZ80AsmBackend.h - GBZ80 Assembler Backend -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_GBZ80_GBZ80ASMBACKEND_H
#define LLVM_LIB_TARGET_GBZ80_GBZ80ASMBACKEND_H

#include "llvm/MC/MCAsmBackend.h"

namespace llvm {

/// Utilities for manipulating generated GBZ80 machine code.
class GBZ80AsmBackend : public MCAsmBackend {
public:
  GBZ80AsmBackend()
      : MCAsmBackend(support::little) {}

  // MCAsmBackend API.
  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override;
  unsigned getNumFixupKinds() const override;
  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;
  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override;
  bool writeNopData(raw_ostream &OS, uint64_t Count) const override;
};

} // end namespace llvm

#endif
