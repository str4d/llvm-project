//===- MCSymbolRGB9.h -  ----------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_MC_MCSYMBOLRGB9_H
#define LLVM_MC_MCSYMBOLRGB9_H

#include "llvm/BinaryFormat/RGB9.h"
#include "llvm/MC/MCSymbol.h"

namespace llvm {
class MCSymbolRGB9 : public MCSymbol {
  RGB9::SymbolType Type = RGB9::SYMBOL_LOCAL;

public:
  MCSymbolRGB9(const StringMapEntry<bool> *Name, bool isTemporary)
      : MCSymbol(SymbolKindRGB9, Name, isTemporary) {}
  static bool classof(const MCSymbol *S) { return S->isRGB9(); }

  void setType(RGB9::SymbolType T) { Type = T; }
  RGB9::SymbolType getType() const { return Type; }

};
}

#endif
