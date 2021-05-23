//===-- llvm/BinaryFormat/RGB9.cpp -------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/BinaryFormat/RGB9.h"

std::string llvm::RGB9::rpnOpToString(RPNOperation Op, std::function<void(raw_string_ostream&, uint32_t)>& symbolizer) {
  std::string FmtBuf;
  raw_string_ostream Fmt(FmtBuf);

  switch (Op.OpCode) {
    case RGB9::RPN_ADD:
      return "+";
    case RGB9::RPN_SUB:
      return "-";
    case RGB9::RPN_MUL:
      return "*";
    case RGB9::RPN_DIV:
      return "/";
    case RGB9::RPN_MOD:
      return "%";
    case RGB9::RPN_NEG:
      return "-()";
    case RGB9::RPN_POW:
      return "**";
    case RGB9::RPN_BINOR:
      return "|";
    case RGB9::RPN_BINAND:
      return "&";
    case RGB9::RPN_XOR:
      return "^";
    case RGB9::RPN_CPL:
      return "~";
    case RGB9::RPN_AND:
      return "&&";
    case RGB9::RPN_OR:
      return "||";
    case RGB9::RPN_NOT:
      return "!";
    case RGB9::RPN_EQ:
      return "==";
    case RGB9::RPN_NEQ:
      return "!=";
    case RGB9::RPN_GT:
      return ">";
    case RGB9::RPN_LT:
      return "<";
    case RGB9::RPN_GTE:
      return ">=";
    case RGB9::RPN_LTE:
      return "<=";
    case RGB9::RPN_LSH:
      return "<<";
    case RGB9::RPN_RSH:
      return ">>";
    case RGB9::RPN_BANK_SYM:
      if (!Op.SymbolOrInt) {
        llvm_unreachable("rpn op missing symbol");
      }
      Fmt << "BANK(";
      symbolizer(Fmt, Op.SymbolOrInt.getValue());
      Fmt << ")";
      break;
    case RGB9::RPN_BANK_SEC:
      if (!Op.SectionName) {
        llvm_unreachable("rpn op missing symbol");
      }
      Fmt << "BANK(";
      Fmt << Op.SectionName.getValue();
      Fmt << ")";
      break;
    case RGB9::RPN_BANK_SELF:
      return "BANK(@)";
    case RGB9::RPN_SIZEOF_SEC:
      if (!Op.SectionName) {
        llvm_unreachable("rpn op missing symbol");
      }
      Fmt << "SIZEOF(";
      Fmt << Op.SectionName.getValue();
      Fmt << ")";
      break;
    case RGB9::RPN_STARTOF_SEC:
      if (!Op.SectionName) {
        llvm_unreachable("rpn op missing symbol");
      }
      Fmt << "STARTOF(";
      Fmt << Op.SectionName.getValue();
      Fmt << ")";
      break;
    case RGB9::RPN_HRAM_CHECK:
      return "HRAM?";
    case RGB9::RPN_RST_CHECK:
      return "RST?";
    case RGB9::RPN_INT:
      if (!Op.SymbolOrInt) {
        llvm_unreachable("rpn op missing symbol");
      }
      Fmt.write_hex(Op.SymbolOrInt.getValue());
      break;
    case RGB9::RPN_SYM:
      if (!Op.SymbolOrInt) {
        llvm_unreachable("rpn op missing symbol");
      }
      symbolizer(Fmt, Op.SymbolOrInt.getValue());
      break;
    default:
      llvm_unreachable("unknown symbol type");
  }

  Fmt.flush();
  return FmtBuf;
}
