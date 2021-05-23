//===- RGB9.h - RGB9 object file format -------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines manifest constants for the RGB9 object file format.
// See: https://rgbds.gbdev.io/docs/v0.5.1/rgbds.5
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_BINARYFORMAT_RGB9_H
#define LLVM_BINARYFORMAT_RGB9_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
namespace RGB9 {

// Object file magic string.
const char Magic[] = {'R', 'G', 'B', '9'};

const uint8_t SECTION_TYPE_MASK = 0x3f;
const uint8_t SECTION_FORM_SHIFT = 6;

// Node types.
enum : uint8_t {
  NODE_REPT = 0,
  NODE_FILE = 1,
  NODE_MACRO = 2,
};

// Symbol types.
enum : uint8_t {
  SYMBOL_LOCAL = 0,
  SYMBOL_IMPORT = 1,
  SYMBOL_EXPORT = 2,
};

// Section types.
enum SectionType : uint8_t {
  SEC_WRAM0 = 0,
  SEC_VRAM = 1,
  SEC_ROMX = 2,
  SEC_ROM0 = 3,
  SEC_HRAM = 4,
  SEC_WRAMX = 5,
  SEC_SRAM = 6,
  SEC_OAM = 7,
};

// Section forms.
enum SectionForm : uint8_t {
  SEC_SIMPLE = 0,
  SEC_FRAGMENT = 1,
  SEC_UNIONIZED = 2,
};

// Patch types
enum : uint8_t {
  P_BYTE = 0,
  P_WORD_LE = 1,
  P_LONG_LE = 2,
  P_BYTE_JR_OFF_VAL = 3,
};

// Assertion types.
enum : uint8_t {
  ASSERT_WARN = 0,  // Prints the message but allows linking to continue.
  ASSERT_ERROR = 0, // Prints the message and evaluates other assertions, but linking fails afterwards.
  ASSERT_FAIL = 0,  // Prints the mesage and immediately fails linking.
};

// RPN operations
enum RpnOp : uint8_t {
  RPN_ADD = 0x00,    // + operator
  RPN_SUB = 0x01,    // - operator
  RPN_MUL = 0x02,    // * operator
  RPN_DIV = 0x03,    // / operator
  RPN_MOD = 0x04,    // % operator
  RPN_NEG = 0x05,    // unary -
  RPN_POW = 0x06,    // ** operator
  RPN_BINOR = 0x10,  // | operator
  RPN_BINAND = 0x11, // & operator
  RPN_XOR = 0x12,    // ^ operator
  RPN_CPL = 0x13,    // unary ~
  RPN_AND = 0x21,    // && comparison
  RPN_OR = 0x22,     // || comparison
  RPN_NOT = 0x23,    // unary !
  RPN_EQ = 0x30,     // == comparison
  RPN_NEQ = 0x31,    // != comparison
  RPN_GT = 0x32,     // > comparison
  RPN_LT = 0x33,     // < comparison
  RPN_GTE = 0x34,    // >= comparison
  RPN_LTE = 0x35,    // <= comparison
  RPN_LSH = 0x40,    // << operator
  RPN_RSH = 0x41,    // >> operator
  RPN_BANK_SYM = 0x50,    // BANK(symbol), a LONG Symbol ID follows, where -1 means PC
  RPN_BANK_SEC = 0x51,    // BANK(section_name), a null-terminated string follows.
  RPN_BANK_SELF = 0x52,   // Current BANK()
  RPN_SIZEOF_SEC = 0x53,  // SIZEOF(section_name), a null-terminated string follows.
  RPN_STARTOF_SEC = 0x54, // STARTOF(section_name), a null-terminated string follows.
  RPN_HRAM_CHECK = 0x60,  // HRAMCheck. Checks if the value is in HRAM, ANDs it with 0xFF.
  RPN_RST_CHECK = 0x61,   // RSTCheck. Checks if the value is a RST vector, ORs it with 0xC7.
  RPN_INT = 0x80, // LONG integer follows.
  RPN_SYM = 0x81, // LONG symbol ID follows.
};

struct ObjectHeader {
  StringRef Magic;
  uint32_t Revision;
  uint32_t NumberOfSymbols;
  uint32_t NumberOfSections;
  uint32_t NumberOfNodes;
  uint32_t NumberOfAssertions;
};

struct Node {
  uint32_t ParentID;
  uint32_t ParentLineNo;
  uint8_t Type;
  // If the node is not a REPT, the node's name; either a file name,
  // or macro name prefixed by its definition file name.
  Optional<StringRef> Name;
  // If the node is a REPT, it also contains the iter counts of all
  // the parent REPTs by increasing depth.
  Optional<ArrayRef<uint32_t>> Iter;
};

struct SymbolData {
  uint32_t SourceFile;
  uint32_t LineNum;
  uint32_t SectionID;
  uint32_t Value;
};

struct Symbol {
  StringRef Name;
  uint8_t Type;
  Optional<SymbolData> Data;
};

struct RPNOperation {
  RpnOp OpCode;
  Optional<uint32_t> SymbolOrInt;
  Optional<StringRef> SectionName;
};

struct RPNExpression {
  std::vector<RPNOperation> Ops;
};

struct SectionPatch {
  uint32_t SourceFile;
  uint32_t LineNo;
  uint32_t Offset;
  uint32_t PCSectionID;
  uint32_t PCOffset;
  uint8_t Type;
  RPNExpression RPN;
};

struct SectionData {
  ArrayRef<uint8_t> Raw;
  std::vector<SectionPatch> Patches;
};

struct Section {
  StringRef Name;
  uint32_t Size;
  SectionType Type;
  SectionForm Form;
  uint32_t Org;
  uint32_t Bank;
  uint8_t Align;
  uint32_t Ofs;
  Optional<SectionData> Data;
};

struct Assertion {
  uint32_t SourceFile;
  uint32_t LineNo;
  uint32_t Offset;
  uint32_t SectionID;
  uint32_t PCOffset;
  uint8_t Type;
  RPNExpression RPN;
  StringRef Message;
};

std::string rpnOpToString(RPNOperation Op, std::function<void(raw_string_ostream&, uint32_t)>& symbolizer);

} // end namespace RGB9
} // end namespace llvm

#endif
