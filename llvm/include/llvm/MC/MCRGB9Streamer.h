//===- MCRGB9Streamer.h - MCStreamer RGB9 Object File Interface -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_MC_MCRGB9STREAMER_H
#define LLVM_MC_MCRGB9STREAMER_H

#include "MCAsmBackend.h"
#include "MCCodeEmitter.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCObjectStreamer.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/DataTypes.h"

namespace llvm {
class MCExpr;
class MCInst;

class MCRGB9Streamer : public MCObjectStreamer {
public:
  MCRGB9Streamer(MCContext &Context, std::unique_ptr<MCAsmBackend> TAB,
                 std::unique_ptr<MCObjectWriter> OW,
                 std::unique_ptr<MCCodeEmitter> Emitter)
      : MCObjectStreamer(Context, std::move(TAB), std::move(OW),
                         std::move(Emitter)) {}

  ~MCRGB9Streamer() override;

  /// state management
  void reset() override {
    MCObjectStreamer::reset();
  }

  /// \name MCStreamer Interface
  /// @{
  bool emitSymbolAttribute(MCSymbol *Symbol, MCSymbolAttr Attribute) override;
  void emitCommonSymbol(MCSymbol *Symbol, uint64_t Size,
                        unsigned ByteAlignment) override;
  void emitZerofill(MCSection *Section, MCSymbol *Symbol = nullptr,
                    uint64_t Size = 0, unsigned ByteAlignment = 0,
                    SMLoc Loc = SMLoc()) override;
  /// @}

private:
  /// \name MCObjectStreamer Interface
  /// @{
  void emitInstToData(const MCInst &Inst, const MCSubtargetInfo &) override;
  /// @}
};

} // end namespace llvm

#endif
