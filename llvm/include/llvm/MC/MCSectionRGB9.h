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
#include "llvm/BinaryFormat/RGB9.h"
#include "llvm/IR/GlobalObject.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/SectionKind.h"

namespace llvm {

class RGB9SectionData {
  // Section type.
  RGB9::SectionType Type;

  // Fixed address for the section. Empty if 0.
  uint16_t Address;

  // Fixed bank for the section. Empty if ~0.
  unsigned Bank;

  // The symbol of this section, if it is a unique section ("@").
  const GlobalObject *Object;

public:
  RGB9SectionData(RGB9::SectionType T, uint16_t A = 0,
                   unsigned B = ~0U,
                   const GlobalObject *O = nullptr) :
    Type(T), Address(A), Bank(B),
    Object(O) {
    // XXX: Validate? We cannot do full validation here without knowing more,
    // like the number of banks.
  }

  bool hasAddress() const { return Address != 0; }
  bool hasBank() const { return Bank != ~0U; }

  RGB9::SectionType getType() const { return Type; }
  uint16_t getAddress() const { return Address; }
  unsigned getBank() const { return Bank; }
  const GlobalObject *getGlobalObject() const { return Object; }

  void setType(RGB9::SectionType T) { Type = T; }
  void setAddress(uint16_t A) { Address = A; }
  void setBank(unsigned B) { Bank = B; }

  void getAsString(SmallVectorImpl<char> &Str) const;
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
