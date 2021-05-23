//===- RGB9.h - RGB9 object file implementation -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the RGB9ObjectFile class, which implements the ObjectFile
// interface for RGB9 files.
//
// See: https://rgbds.gbdev.io/docs/v0.5.1/rgbds.5
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_OBJECT_RGB9_H
#define LLVM_OBJECT_RGB9_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/BinaryFormat/RGB9.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Object/Binary.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/MemoryBuffer.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace llvm {
namespace object {

class RGB9ObjectFile : public ObjectFile {
public:
  RGB9ObjectFile(MemoryBufferRef Object, Error &Err);

  const RGB9::ObjectHeader &getHeader() const;
  const RGB9::RPNExpression &getSectionPatchRPN(const RelocationRef &Ref) const;
  const Optional<RGB9::Symbol> getSymbolWithID(uint32_t SymbolID) const;

  static bool classof(const Binary *v) { return v->isRGB9(); }

  struct ReadContext {
    const uint8_t *Start;
    const uint8_t *Ptr;
    const uint8_t *End;
  };

private:
  const RGB9::Symbol &getSymbol(DataRefImpl Symb) const;
  const RGB9::Section &getSection(DataRefImpl Sec) const;
  const RGB9::SectionPatch &getSectionPatch(DataRefImpl Rel) const;

protected:
  // Overrides from SymbolicFile.
  void moveSymbolNext(DataRefImpl &Symb) const override;
  Expected<uint32_t> getSymbolFlags(DataRefImpl Symb) const override;
  basic_symbol_iterator symbol_begin() const override;
public: // So we can use this in RGB9Dump.cpp
  basic_symbol_iterator symbol_end() const override;

protected:
  // Overrides from ObjectFile for SymbolRef.
  Expected<StringRef> getSymbolName(DataRefImpl Symb) const override;
  Expected<uint64_t> getSymbolAddress(DataRefImpl Symb) const override;
  uint64_t getSymbolValueImpl(DataRefImpl Symb) const override;
  uint64_t getCommonSymbolSizeImpl(DataRefImpl Symb) const override;
  Expected<SymbolRef::Type> getSymbolType(DataRefImpl Symb) const override;
  Expected<section_iterator> getSymbolSection(DataRefImpl Symb) const override;

  // Overrides from ObjectFile for SectionRef.
  void moveSectionNext(DataRefImpl &Sec) const override;
  Expected<StringRef> getSectionName(DataRefImpl Sec) const override;
  uint64_t getSectionAddress(DataRefImpl Sec) const override;
  uint64_t getSectionIndex(DataRefImpl Sec) const override;
  uint64_t getSectionSize(DataRefImpl Sec) const override;
  Expected<ArrayRef<uint8_t>> getSectionContents(DataRefImpl Sec) const override;
  uint64_t getSectionAlignment(DataRefImpl Sec) const override;
  bool isSectionCompressed(DataRefImpl Sec) const override;
  bool isSectionText(DataRefImpl Sec) const override;
  bool isSectionData(DataRefImpl Sec) const override;
  bool isSectionBSS(DataRefImpl Sec) const override;
  bool isSectionVirtual(DataRefImpl Sec) const override;
  relocation_iterator section_rel_begin(DataRefImpl Sec) const override;
  relocation_iterator section_rel_end(DataRefImpl Sec) const override;

  // Overrides from ObjectFile for RelocationRef.
  void moveRelocationNext(DataRefImpl &Rel) const override;
  uint64_t getRelocationOffset(DataRefImpl Rel) const override;
  symbol_iterator getRelocationSymbol(DataRefImpl Rel) const override;
  uint64_t getRelocationType(DataRefImpl Rel) const override;
  void getRelocationTypeName(DataRefImpl Rel,
                             SmallVectorImpl<char> &Result) const override;

  section_iterator section_begin() const override;
  section_iterator section_end() const override;
  uint8_t getBytesInAddress() const override;
  StringRef getFileFormatName() const override;
  Triple::ArchType getArch() const override;
  Expected<uint64_t> getStartAddress() const override;
  SubtargetFeatures getFeatures() const override;
  bool isRelocatableObject() const override;

private:
  RGB9::ObjectHeader Header;
  std::vector<RGB9::Node> Nodes;
  std::vector<RGB9::Symbol> Symbols;
  std::vector<RGB9::Section> Sections;
  std::vector<RGB9::Assertion> Assertions;
};

} // end namespace object
} // end namespace llvm

#endif // LLVM_OBJECT_RGB9_H
