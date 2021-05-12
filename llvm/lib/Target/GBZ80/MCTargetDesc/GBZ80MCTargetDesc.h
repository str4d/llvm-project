//===-- GBZ80MCTargetDesc.h - GBZ80 Target Descriptions -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides GBZ80 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_GBZ80_MCTARGET_DESC_H
#define LLVM_GBZ80_MCTARGET_DESC_H

#include "llvm/Support/DataTypes.h"
#include <memory>

namespace llvm {

class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectWriter;
class MCRegisterInfo;
class MCTargetOptions;
class StringRef;
class Target;
class Triple;
class raw_pwrite_stream;

Target &getTheGBZ80Target();

/// Creates a machine code emitter for GBZ80.
MCCodeEmitter *createGBZ80MCCodeEmitter(const MCInstrInfo &MCII,
                                      const MCRegisterInfo &MRI,
                                      MCContext &Ctx);

/// Creates an assembly backend for GBZ80.
MCAsmBackend *createGBZ80AsmBackend(const Target &T, const MCRegisterInfo &MRI,
                                  const Triple &TT, StringRef CPU,
                                  const llvm::MCTargetOptions &TO);

/// Creates an ELF object writer for GBZ80.
std::unique_ptr<MCObjectWriter> createGBZ80ELFObjectWriter(raw_pwrite_stream &OS, uint8_t OSABI);

} // end namespace llvm

#define GET_REGINFO_ENUM
#include "GBZ80GenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "GBZ80GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "GBZ80GenSubtargetInfo.inc"

#endif // LLVM_GBZ80_MCTARGET_DESC_H
