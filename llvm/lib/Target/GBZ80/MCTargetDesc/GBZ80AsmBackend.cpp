//===-- GBZ80AsmBackend.cpp - GBZ80 Assembler Backend -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/GBZ80AsmBackend.h"
#include "MCTargetDesc/GBZ80MCTargetDesc.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

std::unique_ptr<MCObjectTargetWriter>
GBZ80AsmBackend::createObjectTargetWriter() const {
  return createGBZ80RGB9ObjectWriter();
}

unsigned GBZ80AsmBackend::getNumFixupKinds() const {
  llvm_unreachable("Not yet implemented for GBZ80");
}

void GBZ80AsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                                 const MCValue &Target,
                                 MutableArrayRef<char> Data,
                                 uint64_t Value, bool IsResolved,
                                 const MCSubtargetInfo *STI) const {
  llvm_unreachable("Not yet implemented for GBZ80");
}

bool GBZ80AsmBackend::fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                                           const MCRelaxableFragment *DF,
                                           const MCAsmLayout &Layout) const {
  llvm_unreachable("Not yet implemented for GBZ80");
}

bool GBZ80AsmBackend::writeNopData(raw_ostream &OS, uint64_t Count) const {
  llvm_unreachable("Not yet implemented for GBZ80");
}

MCAsmBackend *llvm::createGBZ80AsmBackend(const Target &T,
                                          const MCSubtargetInfo &STI,
                                          const MCRegisterInfo &MRI,
                                          const MCTargetOptions &TO) {
  return new GBZ80AsmBackend();
}
