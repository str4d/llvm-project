//===-- RGB9Dump.cpp - RGB9-specific dumper ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the RGB9-specific dumper for llvm-objdump.
///
//===----------------------------------------------------------------------===//

#include "RGB9Dump.h"

#include "llvm-objdump.h"
#include "llvm/BinaryFormat/RGB9.h"
#include "llvm/Object/RGB9ObjectFile.h"

using namespace llvm;
using namespace llvm::object;

void objdump::printRGB9FileHeader(const object::ObjectFile *Obj) {
  const auto *File = dyn_cast<const RGB9ObjectFile>(Obj);
  const auto Header = File->getHeader();

  outs() << "Program Header:\n";
  outs() << "  Revision: 0x";
  outs().write_hex(Header.Revision);
  outs() << "\n";
  outs() << "  Symbols: " << Header.NumberOfSymbols << "\n";
  outs() << "  Sections: " << Header.NumberOfSections << "\n";
  outs() << "  Assertions: " << Header.NumberOfAssertions << "\n";
}

Error objdump::getRGB9RelocationValueString(const RGB9ObjectFile *Obj,
                                            const RelocationRef &RelRef,
                                            SmallVectorImpl<char> &Result) {
  const RGB9::RPNExpression &Expr = Obj->getSectionPatchRPN(RelRef);
  std::string FmtBuf;
  raw_string_ostream Fmt(FmtBuf);

  std::function<void(raw_string_ostream&, uint32_t)> symbolizer =
    [&Obj](raw_string_ostream &RPNFmt, uint32_t SymbolID)
  {
    Optional<RGB9::Symbol> S = Obj->getSymbolWithID(SymbolID);
    if (S) {
      RPNFmt << S.getValue().Name;
    } else {
      RPNFmt << "Sym#";
      RPNFmt << SymbolID;
    }
  };

  bool AddSpace = false;
  for (const auto &Oper : Expr.Ops) {
    if (AddSpace) {
      Fmt << " ";
    } else {
      AddSpace = true;
    }
    Fmt << RGB9::rpnOpToString(Oper, symbolizer);
  }

  Fmt.flush();
  Result.append(FmtBuf.begin(), FmtBuf.end());
  return Error::success();
}
