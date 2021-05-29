//===- MCSectionRGB9.h - RGB9 Machine Code Sections -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the MCSectionRGB9 class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_MC_MCSECTIONRGB9_H
#define LLVM_MC_MCSECTIONRGB9_H

#include "llvm/ADT/StringRef.h"
#include "llvm/IR/GlobalObject.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/SectionKind.h"

namespace llvm {

enum GBSectionType {
  ST_ROM0 = 0,
  ST_ROMX,
  ST_VRAM,
  ST_SRAM,
  ST_WRAM0,
  ST_WRAMX,
  ST_OAM,
  ST_HRAM,
  ST_NONE
};

class RGB9SectionData {
  // Section type.
  GBSectionType Type;

  // Fixed address for the section. Empty if 0.
  uint16_t Address;

  // Fixed bank for the section. Empty if ~0.
  unsigned Bank;

  // Alignment for the section. This is actual alignment and not log2.
  // Empty if 1.
  unsigned Alignment;

  // The symbol of this section, if it is a unique section ("@").
  const GlobalObject *Object;

public:
  RGB9SectionData(GBSectionType T = ST_NONE, uint16_t A = 0,
                   unsigned B = ~0U, unsigned Align = 1,
                   const GlobalObject *O = nullptr) :
    Type(T), Address(A), Bank(B), Alignment(Align),
    Object(O) {
    // XXX: Validate? We cannot do full validation here without knowing more,
    // like the number of banks.
  }

  bool hasAddress() const { return Address != 0; }
  bool hasBank() const { return Bank != ~0U; }
  bool hasAlignment() const { return Alignment > 1; }

  GBSectionType getType() const { return Type; }
  uint16_t getAddress() const { return Address; }
  unsigned getBank() const { return Bank; }
  unsigned getAlignment() const { return Alignment; }
  const GlobalObject *getGlobalObject() const { return Object; }

  void setType(GBSectionType T) { Type = T; }
  void setAddress(uint16_t A) { Address = A; }
  void setBank(unsigned B) { Bank = B; }
  void setAlignment(unsigned Align) { Alignment = Align; }

  void getAsString(SmallVectorImpl<char> &Str) const;

  static bool get(StringRef Specifier, RGB9SectionData &Data);
};

// Provide an ordering.
inline bool operator <(const RGB9SectionData &A, const RGB9SectionData &B) {
  if (A.getGlobalObject() < B.getGlobalObject()) return true;
  if (A.getGlobalObject() > B.getGlobalObject()) return false;
  if (A.getType() < B.getType()) return true;
  if (A.getType() > B.getType()) return false;
  if (A.getAddress() < B.getAddress()) return true;
  if (A.getAddress() > B.getAddress()) return false;
  if (A.getBank() < B.getBank()) return true;
  if (A.getBank() > B.getBank()) return false;
  if (A.getAlignment() < B.getAlignment()) return true;
  if (A.getAlignment() > B.getAlignment()) return false;
  return false; // equal
}

/// This represents a section on a Game Boy cartridge.
class MCSectionRGB9 final : public MCSection {
public:

  // If Name is empty, the section is named @.
  // TODO: Or? Should we come up with something else for this?
  MCSectionRGB9(StringRef Name, SectionKind K, RGB9SectionData D, MCSymbol *Begin)
      : MCSection(SV_RGB9, Name, K, Begin), Data(D) { }

  void PrintSwitchToSection(const MCAsmInfo &MAI, const Triple &T,
                            raw_ostream &OS,
                            const MCExpr *Subsection) const override;
  bool UseCodeAlign() const override;
  bool isVirtualSection() const override;

  static bool classof(const MCSection *S) { return S->getVariant() == SV_RGB9; }

private:
  RGB9SectionData Data;

};

} // end namespace llvm

#endif // LLVM_MC_MCSECTIONRGB9_H
