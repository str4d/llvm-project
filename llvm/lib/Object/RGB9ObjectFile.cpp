//===- RGB9ObjectFile.cpp - RGB9 object file implementation ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/BinaryFormat/RGB9.h"
#include "llvm/Object/Binary.h"
#include "llvm/Object/RGB9ObjectFile.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/Error.h"

#define DEBUG_TYPE "rgb9-object"

using namespace llvm;
using namespace object;

static Error readUint8(RGB9ObjectFile::ReadContext &Ctx,
                       uint8_t &Result,
                       StringRef ErrMsg = "EOF while reading uint8") {
  if (Ctx.Ptr == Ctx.End)
    return make_error<StringError>(ErrMsg, object_error::parse_failed);
  Result = *Ctx.Ptr++;
  return Error::success();
}

static Error readUint32(RGB9ObjectFile::ReadContext &Ctx,
                        uint32_t &Result,
                        StringRef ErrMsg = "EOF while reading uint32") {
  if (Ctx.Ptr + 4 > Ctx.End) {
    return make_error<StringError>(ErrMsg, object_error::parse_failed);
  }
  Result = support::endian::read32le(Ctx.Ptr);
  Ctx.Ptr += 4;
  return Error::success();
}

static Error readString(RGB9ObjectFile::ReadContext &Ctx, StringRef &Result,
                        StringRef ErrMsg = "EOF while reading string") {
  uint32_t StringLen = 0;
  while (*(Ctx.Ptr + StringLen) != 0) {
    StringLen++;
    if (Ctx.Ptr + StringLen > Ctx.End)
      return make_error<StringError>(ErrMsg, object_error::parse_failed);
  }
  Result = StringRef(reinterpret_cast<const char *>(Ctx.Ptr), StringLen);
  // Ensure we skip past the NUL.
  Ctx.Ptr += StringLen + 1;
  return Error::success();
}

static Error readArrayUint8(RGB9ObjectFile::ReadContext &Ctx, uint32_t Size,
                            ArrayRef<uint8_t> &Result,
                            StringRef ErrMsg = "EOF while reading array") {
  if (Ctx.Ptr + Size > Ctx.End)
    return make_error<StringError>(ErrMsg, object_error::parse_failed);
  Result = ArrayRef<uint8_t>(Ctx.Ptr, Size);
  Ctx.Ptr += Size;
  return Error::success();
}

static Error readArrayUint32(RGB9ObjectFile::ReadContext &Ctx, uint32_t Size,
                            ArrayRef<uint32_t> &Result,
                            StringRef ErrMsg = "EOF while reading array") {
  if (Ctx.Ptr + (Size * 4) > Ctx.End)
    return make_error<StringError>(ErrMsg, object_error::parse_failed);
  Result = ArrayRef<uint32_t>(reinterpret_cast<const uint32_t *>(Ctx.Ptr), Size);
  Ctx.Ptr += (Size * 4);
  return Error::success();
}

static void readRPN(RGB9ObjectFile::ReadContext &Ctx, Error &Err,
                    RGB9::RPNExpression &Result,
                    StringRef ErrMsg = "EOF while reading RPN") {
  ErrorAsOutParameter ErrAsOutParam(&Err);

  uint32_t RPNSize;
  ArrayRef<uint8_t> RPN;
  if (
    (Err = readUint32(Ctx, RPNSize, "EOF while reading RPNSize")) ||
    (Err = readArrayUint8(Ctx, RPNSize, RPN, ErrMsg))
  ) {
    return;
  }

  RGB9ObjectFile::ReadContext RPNCtx;
  RPNCtx.Start = RPN.begin();
  RPNCtx.Ptr = RPNCtx.Start;
  RPNCtx.End = RPN.end();

  RGB9::RPNExpression Expr;
  while (RPNCtx.Ptr < RPNCtx.End) {
    RGB9::RPNOperation Oper;
    uint8_t OpCode;
    if ((Err = readUint8(RPNCtx, OpCode))) {
      return;
    }
    uint32_t SymbolOrInt;
    StringRef SectionName;
    switch (OpCode) {
      case RGB9::RPN_ADD:
      case RGB9::RPN_SUB:
      case RGB9::RPN_MUL:
      case RGB9::RPN_DIV:
      case RGB9::RPN_MOD:
      case RGB9::RPN_NEG:
      case RGB9::RPN_POW:
      case RGB9::RPN_BINOR:
      case RGB9::RPN_BINAND:
      case RGB9::RPN_XOR:
      case RGB9::RPN_CPL:
      case RGB9::RPN_AND:
      case RGB9::RPN_OR:
      case RGB9::RPN_NOT:
      case RGB9::RPN_EQ:
      case RGB9::RPN_NEQ:
      case RGB9::RPN_GT:
      case RGB9::RPN_LT:
      case RGB9::RPN_GTE:
      case RGB9::RPN_LTE:
      case RGB9::RPN_LSH:
      case RGB9::RPN_RSH:
      case RGB9::RPN_BANK_SELF:
      case RGB9::RPN_HRAM_CHECK:
      case RGB9::RPN_RST_CHECK:
        Oper.OpCode = static_cast<RGB9::RpnOp>(OpCode);
        break;
      case RGB9::RPN_BANK_SYM:
      case RGB9::RPN_SYM:
      case RGB9::RPN_INT:
        if ((Err = readUint32(RPNCtx, SymbolOrInt))) {
          return;
        }
        Oper.OpCode = static_cast<RGB9::RpnOp>(OpCode);
        Oper.SymbolOrInt = SymbolOrInt;
        break;
      case RGB9::RPN_BANK_SEC:
      case RGB9::RPN_SIZEOF_SEC:
      case RGB9::RPN_STARTOF_SEC:
        if ((Err = readString(RPNCtx, SectionName))) {
          return;
        }
        Oper.OpCode = static_cast<RGB9::RpnOp>(OpCode);
        Oper.SectionName = SectionName;
        break;
      default:
        Err = make_error<StringError>("invalid RPN opcode", object_error::parse_failed);
        if (Err) {
          return;
        }
    }
    Expr.Ops.push_back(Oper);
  }

  Result = Expr;
}

static void readNode(RGB9::Node &Node,
                     RGB9ObjectFile::ReadContext &Ctx, Error &Err) {
  ErrorAsOutParameter ErrAsOutParam(&Err);

  if (
    (Err = readUint32(Ctx, Node.ParentID)) ||
    (Err = readUint32(Ctx, Node.ParentLineNo)) ||
    (Err = readUint8(Ctx, Node.Type))
  ) {
    return;
  }

  if (Node.Type != RGB9::NODE_REPT) {
    StringRef Name;
    if ((Err = readString(Ctx, Name))) {
      return;
    } else {
      LLVM_DEBUG(dbgs() << "readNode type=" << (int)Node.Type << " name=" << Name << "\n");
      Node.Name = Name;
    }
  } else {
    uint32_t Depth;
    ArrayRef<uint32_t> Iter;
    if (
      (Err = readUint32(Ctx, Depth)) ||
      (Err = readArrayUint32(Ctx, Depth, Iter))
    ) {
      return;
    } else {
      LLVM_DEBUG(dbgs() << "readNode REPT depth=" << Depth << "\n");
      Node.Iter = Iter;
    }
  }
}

static void readSymbol(RGB9::Symbol &Symbol,
                       RGB9ObjectFile::ReadContext &Ctx, Error &Err) {
  ErrorAsOutParameter ErrAsOutParam(&Err);

  if (
    (Err = readString(Ctx, Symbol.Name)) ||
    (Err = readUint8(Ctx, Symbol.Type))
  ) {
    return;
  }
  LLVM_DEBUG(dbgs() << "readSymbol type=" << (int)Symbol.Type << " name=" << Symbol.Name << "\n");

  if ((Symbol.Type & 0x7F) != RGB9::SYMBOL_IMPORT) {
    RGB9::SymbolData Data;
    if (
      (Err = readUint32(Ctx, Data.SourceFile)) ||
      (Err = readUint32(Ctx, Data.LineNum)) ||
      (Err = readUint32(Ctx, Data.SectionID)) ||
      (Err = readUint32(Ctx, Data.Value))
    ) {
      return;
    } else {
      Symbol.Data = Data;
    }
  }
}

static Error checkSection(RGB9::Section &Section, uint8_t Type) {
  uint8_t SectionType = Type & RGB9::SECTION_TYPE_MASK;
  if (SectionType > RGB9::SEC_OAM)
    return make_error<StringError>("unsupported section type",
                                   object_error::parse_failed);
  Section.Type = static_cast<RGB9::SectionType>(SectionType);

  uint8_t SectionForm = Type >> RGB9::SECTION_FORM_SHIFT;
  if (SectionForm > RGB9::SEC_UNIONIZED)
    return make_error<StringError>("section type bits 6 and 7 both set",
                                   object_error::parse_failed);
  Section.Form = static_cast<RGB9::SectionForm>(SectionForm);

  if (Section.Ofs >= (1U << Section.Align))
    return make_error<StringError>("alignment offset too large",
                                   object_error::parse_failed);

  return Error::success();
}

static void readSection(RGB9::Section &Section,
                        RGB9ObjectFile::ReadContext &Ctx, Error &Err) {
  ErrorAsOutParameter ErrAsOutParam(&Err);

  uint8_t Type;
  if (
    (Err = readString(Ctx, Section.Name)) ||
    (Err = readUint32(Ctx, Section.Size)) ||
    (Err = readUint8(Ctx, Type)) ||
    (Err = readUint32(Ctx, Section.Org)) ||
    (Err = readUint32(Ctx, Section.Bank)) ||
    (Err = readUint8(Ctx, Section.Align)) ||
    (Err = readUint32(Ctx, Section.Ofs)) ||
    (Err = checkSection(Section, Type))
  ) {
    return;
  }
  LLVM_DEBUG(dbgs() << "readSection type=" << (int)Section.Type << " name=" << Section.Name << "\n");

  if (Section.Type == RGB9::SEC_ROMX || Section.Type == RGB9::SEC_ROM0) {
    RGB9::SectionData Data;
    uint32_t NumberOfPatches;
    if (
      (Err = readArrayUint8(Ctx, Section.Size, Data.Raw)) ||
      (Err = readUint32(Ctx, NumberOfPatches))
    ) {
      return;
    }

    RGB9::SectionPatch Patch;
    while (Data.Patches.size() < NumberOfPatches) {
      if (
        (Err = readUint32(Ctx, Patch.SourceFile)) ||
        (Err = readUint32(Ctx, Patch.LineNo)) ||
        (Err = readUint32(Ctx, Patch.Offset)) ||
        (Err = readUint32(Ctx, Patch.PCSectionID)) ||
        (Err = readUint32(Ctx, Patch.PCOffset)) ||
        (Err = readUint8(Ctx, Patch.Type))
      ) {
        return;
      }
      readRPN(Ctx, Err, Patch.RPN, "EOF while reading patch RPN");
      if (Err) {
        return;
      }
      Data.Patches.push_back(Patch);
    }

    Section.Data = Data;
  }
}

static void readAssertion(RGB9::Assertion &Assertion,
                          RGB9ObjectFile::ReadContext &Ctx, Error &Err) {
  ErrorAsOutParameter ErrAsOutParam(&Err);

  if (
    (Err = readUint32(Ctx, Assertion.SourceFile)) ||
    (Err = readUint32(Ctx, Assertion.LineNo)) ||
    (Err = readUint32(Ctx, Assertion.Offset)) ||
    (Err = readUint32(Ctx, Assertion.SectionID)) ||
    (Err = readUint32(Ctx, Assertion.PCOffset)) ||
    (Err = readUint8(Ctx, Assertion.Type))
  ) {
    return;
  }
  readRPN(Ctx, Err, Assertion.RPN, "EOF while reading assertion RPN");
  if (Err || (Err = readString(Ctx, Assertion.Message))) {
    return;
  }
}

RGB9ObjectFile::RGB9ObjectFile(MemoryBufferRef Buffer, Error &Err)
    : ObjectFile(Binary::ID_RGB9, Buffer) {
  ErrorAsOutParameter ErrAsOutParam(&Err);
  Header.Magic = getData().substr(0, 4);
  if (Header.Magic != StringRef("RGB9", 4)) {
    Err = make_error<StringError>("invalid magic number",
                                  object_error::parse_failed);
    return;
  }

  ReadContext Ctx;
  Ctx.Start = getData().bytes_begin();
  Ctx.Ptr = Ctx.Start + 4;
  Ctx.End = Ctx.Start + getData().size();

  if (
    (Err = readUint32(Ctx, Header.Revision, "missing revision number")) ||
    (Err = readUint32(Ctx, Header.NumberOfSymbols, "missing number of symbols")) ||
    (Err = readUint32(Ctx, Header.NumberOfSections, "missing number of sections")) ||
    (Err = readUint32(Ctx, Header.NumberOfNodes, "missing number of nodes"))
  ) {
    return;
  }

  RGB9::Node Node;
  while (Nodes.size() < Header.NumberOfNodes) {
    readNode(Node, Ctx, Err);
    if (Err)
      return;
    Nodes.push_back(Node);
  }

  RGB9::Symbol Symbol;
  while (Symbols.size() < Header.NumberOfSymbols) {
    readSymbol(Symbol, Ctx, Err);
    if (Err)
      return;
    Symbols.push_back(Symbol);
  }

  RGB9::Section Section;
  while (Sections.size() < Header.NumberOfSections) {
    readSection(Section, Ctx, Err);
    if (Err)
      return;
    Sections.push_back(Section);
  }

  if ((Err = readUint32(Ctx, Header.NumberOfAssertions, "missing number of assertions"))) {
    return;
  }

  RGB9::Assertion Assertion;
  while (Assertions.size() < Header.NumberOfAssertions) {
    readAssertion(Assertion, Ctx, Err);
    if (Err)
      return;
    Assertions.push_back(Assertion);
  }
}

const RGB9::ObjectHeader &RGB9ObjectFile::getHeader() const {
  return Header;
}

const RGB9::RPNExpression &
RGB9ObjectFile::getSectionPatchRPN(const RelocationRef &RelRef) const {
  return getSectionPatch(RelRef.getRawDataRefImpl()).RPN;
}

const Optional<RGB9::Symbol> RGB9ObjectFile::getSymbolWithID(uint32_t SymbolID) const {
  if (SymbolID < Symbols.size()) {
    return Symbols[SymbolID];
  } else {
    return None;
  }
}

const RGB9::Symbol &RGB9ObjectFile::getSymbol(DataRefImpl Symb) const {
  assert(Symb.d.b < Symbols.size());
  return Symbols[Symb.d.b];
}

const RGB9::Section &RGB9ObjectFile::getSection(DataRefImpl Sec) const {
  assert(Sec.d.a < Sections.size());
  return Sections[Sec.d.a];
}

const RGB9::SectionPatch &
RGB9ObjectFile::getSectionPatch(DataRefImpl Rel) const {
  assert(Rel.d.a < Sections.size());
  const RGB9::Section &Sec = Sections[Rel.d.a];
  assert(Sec.Data);
  const RGB9::SectionData &Data = Sec.Data.getValue();
  assert(Rel.d.b < Data.Patches.size());
  return Data.Patches[Rel.d.b];
}

void RGB9ObjectFile::moveSymbolNext(DataRefImpl &Symb) const { Symb.d.b++; }

Expected<uint32_t> RGB9ObjectFile::getSymbolFlags(DataRefImpl Symb) const {
  const RGB9::Symbol &S = getSymbol(Symb);
  uint32_t Result = SymbolRef::SF_None;

  if (!S.Name.contains('.'))
    Result |= SymbolRef::SF_Global;
  if (S.Type == RGB9::SYMBOL_IMPORT)
    Result |= SymbolRef::SF_Undefined;
  if (S.Type == RGB9::SYMBOL_EXPORT)
    Result |= SymbolRef::SF_Exported;
  if (S.Data && S.Data.getValue().SectionID == (uint32_t)-1)
    Result |= SymbolRef::SF_Const;

  return Result;
}

basic_symbol_iterator RGB9ObjectFile::symbol_begin() const {
  DataRefImpl Symb;
  Symb.d.a = 1; // Arbitrary non-zero value so that Symb.p is non-null
  Symb.d.b = 0; // Symbol index
  return BasicSymbolRef(Symb, this);
}

basic_symbol_iterator RGB9ObjectFile::symbol_end() const {
  DataRefImpl Symb;
  Symb.d.a = 1; // Arbitrary non-zero value so that Symb.p is non-null
  Symb.d.b = Symbols.size(); // Symbol index
  return BasicSymbolRef(Symb, this);
}

Expected<StringRef> RGB9ObjectFile::getSymbolName(DataRefImpl Symb) const {
  return getSymbol(Symb).Name;
}

Expected<uint64_t> RGB9ObjectFile::getSymbolAddress(DataRefImpl Symb) const {
  return getSymbolValue(Symb);
}

uint64_t RGB9ObjectFile::getSymbolValueImpl(DataRefImpl Symb) const {
  return getSymbol(Symb).Data.getValue().Value;
}

uint64_t RGB9ObjectFile::getCommonSymbolSizeImpl(DataRefImpl Symb) const {
  uint64_t Result = 0;
  llvm_unreachable("Not yet implemented!");
  return Result;
}

Expected<SymbolRef::Type>
RGB9ObjectFile::getSymbolType(DataRefImpl Symb) const {
  return SymbolRef::ST_Unknown;
}

Expected<section_iterator>
RGB9ObjectFile::getSymbolSection(DataRefImpl Symb) const {
  const RGB9::Symbol &S = getSymbol(Symb);
  if (!S.Data || S.Data.getValue().SectionID == (uint32_t)-1) {
    return section_end();
  }

  DataRefImpl Ref;
  Ref.d.a = S.Data.getValue().SectionID;
  return section_iterator(SectionRef(Ref, this));
}

void RGB9ObjectFile::moveSectionNext(DataRefImpl &Sec) const { Sec.d.a++; }

Expected<StringRef> RGB9ObjectFile::getSectionName(DataRefImpl Sec) const {
  return getSection(Sec).Name;
}

uint64_t RGB9ObjectFile::getSectionAddress(DataRefImpl Sec) const {
  int32_t Addr = getSection(Sec).Org;
  return Addr == -1 ? 0 : Addr;
}

uint64_t RGB9ObjectFile::getSectionIndex(DataRefImpl Sec) const {
  return Sec.d.a;
}

uint64_t RGB9ObjectFile::getSectionSize(DataRefImpl Sec) const {
  return getSection(Sec).Size;
}

Expected<ArrayRef<uint8_t>>
RGB9ObjectFile::getSectionContents(DataRefImpl Sec) const {
  const RGB9::Section &S = getSection(Sec);
  if (S.Data) {
    return S.Data.getValue().Raw;
  } else {
    return make_error<BinaryError>();
  }
}

uint64_t RGB9ObjectFile::getSectionAlignment(DataRefImpl Sec) const {
  const RGB9::Section &S = getSection(Sec);
  return (1U << S.Align) + S.Ofs;
}

bool RGB9ObjectFile::isSectionCompressed(DataRefImpl Sec) const { return false; }

bool RGB9ObjectFile::isSectionText(DataRefImpl Sec) const {
  // Without some convention, we can't distinguish .text from .data,
  // so assume it's all .text
  return !isSectionVirtual(Sec);
}

bool RGB9ObjectFile::isSectionData(DataRefImpl Sec) const {
  // TODO: Decide on a naming convention for identifying .data
  return false;
}

bool RGB9ObjectFile::isSectionBSS(DataRefImpl Sec) const { return false; }

bool RGB9ObjectFile::isSectionVirtual(DataRefImpl Sec) const {
  const RGB9::Section &S = getSection(Sec);
  // Only ROM0 and ROMX can contain data in the object file.
  return S.Type != RGB9::SEC_ROM0 && S.Type != RGB9::SEC_ROMX;
}

relocation_iterator RGB9ObjectFile::section_rel_begin(DataRefImpl Sec) const {
  DataRefImpl RelocRef;
  RelocRef.d.a = Sec.d.a;
  RelocRef.d.b = 0;
  return relocation_iterator(RelocationRef(RelocRef, this));
}

relocation_iterator RGB9ObjectFile::section_rel_end(DataRefImpl Sec) const {
  const RGB9::Section &S = getSection(Sec);
  DataRefImpl RelocRef;
  RelocRef.d.a = Sec.d.a;
  if (S.Data) {
    RelocRef.d.b = S.Data.getValue().Patches.size();
  } else {
    RelocRef.d.b = 0;
  }
  return relocation_iterator(RelocationRef(RelocRef, this));
}

void RGB9ObjectFile::moveRelocationNext(DataRefImpl &Rel) const { Rel.d.b++; }

uint64_t RGB9ObjectFile::getRelocationOffset(DataRefImpl Rel) const {
  return getSectionPatch(Rel).Offset;
}

symbol_iterator RGB9ObjectFile::getRelocationSymbol(DataRefImpl Rel) const {
  llvm_unreachable("Not yet implemented!");
  return symbol_iterator(SymbolRef());
}

uint64_t RGB9ObjectFile::getRelocationType(DataRefImpl Rel) const {
  return getSectionPatch(Rel).Type;
}

void RGB9ObjectFile::getRelocationTypeName(
    DataRefImpl Rel, SmallVectorImpl<char> &Result) const {
  StringRef Res;
  switch (getRelocationType(Rel)) {
  case RGB9::P_BYTE:
    Res = "P_BYTE";
    break;
  case RGB9::P_WORD_LE:
    Res = "P_WORD_LE";
    break;
  case RGB9::P_LONG_LE:
    Res = "P_LONG_LE";
    break;
  case RGB9::P_BYTE_JR_OFF_VAL:
    Res = "P_BYTE_JR_OFF_VAL";
    break;
  default:
    Res = "Unknown";
  }
  Result.append(Res.begin(), Res.end());
}

section_iterator RGB9ObjectFile::section_begin() const {
  DataRefImpl Ref;
  Ref.d.a = 0;
  return section_iterator(SectionRef(Ref, this));
}

section_iterator RGB9ObjectFile::section_end() const {
  DataRefImpl Ref;
  Ref.d.a = Sections.size();
  return section_iterator(SectionRef(Ref, this));
}

uint8_t RGB9ObjectFile::getBytesInAddress() const { return 2; }

StringRef RGB9ObjectFile::getFileFormatName() const { return "RGBDS v9"; }

Triple::ArchType RGB9ObjectFile::getArch() const { return Triple::gbz80; }

Expected<uint64_t> RGB9ObjectFile::getStartAddress() const {
  // GBZ80 execution begins at $100.
  return 0x100;
}

SubtargetFeatures RGB9ObjectFile::getFeatures() const {
  return SubtargetFeatures();
}

bool RGB9ObjectFile::isRelocatableObject() const { return true; }

Expected<std::unique_ptr<RGB9ObjectFile>>
ObjectFile::createRGB9ObjectFile(MemoryBufferRef Buffer) {
  Error Err = Error::success();
  auto ObjectFile = std::make_unique<RGB9ObjectFile>(Buffer, Err);
  if (Err)
    return std::move(Err);

  return std::move(ObjectFile);
}
