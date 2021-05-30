//===- lib/MC/MCRGB9Streamer.cpp - RGB9 Object Output ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file assembles .s files and emits RGB9 .o object files.
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/MCRGB9Streamer.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

MCRGB9Streamer::~MCRGB9Streamer() = default; // anchor.

bool MCRGB9Streamer::emitSymbolAttribute(MCSymbol *S, MCSymbolAttr Attribute) {
  llvm_unreachable("Not yet implemented for RGB9");
}

void MCRGB9Streamer::emitCommonSymbol(MCSymbol *S, uint64_t Size,
                                      unsigned ByteAlignment) {
  llvm_unreachable("Common symbols are not yet implemented for RGB9");
}

void MCRGB9Streamer::emitZerofill(MCSection *Section, MCSymbol *Symbol,
                                  uint64_t Size, unsigned ByteAlignment,
                                  SMLoc Loc) {
  llvm_unreachable("Not yet implemented for RGB9");
}

void MCRGB9Streamer::emitInstToData(const MCInst &Inst,
                                    const MCSubtargetInfo &STI) {
  llvm_unreachable("Not yet implemented for RGB9");
}

MCStreamer *llvm::createRGB9Streamer(MCContext &Context,
                                     std::unique_ptr<MCAsmBackend> &&MAB,
                                     std::unique_ptr<MCObjectWriter> &&OW,
                                     std::unique_ptr<MCCodeEmitter> &&CE,
                                     bool RelaxAll) {
  MCRGB9Streamer *S =
      new MCRGB9Streamer(Context, std::move(MAB), std::move(OW), std::move(CE));
  if (RelaxAll)
    S->getAssembler().setRelaxAll(true);
  return S;
}
