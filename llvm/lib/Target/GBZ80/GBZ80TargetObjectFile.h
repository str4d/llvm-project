//===-- GBZ80TargetObjectFile.h - GBZ80 Object Info -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_GBZ80_TARGET_OBJECT_FILE_H
#define LLVM_GBZ80_TARGET_OBJECT_FILE_H

#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCSection.h"
#include "llvm/Target/TargetLoweringObjectFile.h"

#include <map>

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

class GBZ80SectionData {
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
  GBZ80SectionData(GBSectionType T = ST_NONE, uint16_t A = 0,
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

  static bool get(StringRef Specifier, GBZ80SectionData &Data);
};

// Provide an ordering.
inline bool operator <(const GBZ80SectionData &A, const GBZ80SectionData &B) {
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

class MCSectionGBZ80 : public MCSection {
public:

  // If Name is empty, the section is named @.
  // TODO: Or? Should we come up with something else for this?
  MCSectionGBZ80(StringRef Name, SectionKind K, GBZ80SectionData D, MCSymbol *Begin)
  : MCSection(SV_GBZ80, Name, K, Begin), Data(D) { }

  void PrintSwitchToSection(const MCAsmInfo & MAI, const Triple & T,
    raw_ostream & OS, const MCExpr * Subsection) const override;
  bool UseCodeAlign() const override;
  bool isVirtualSection() const override;

  static bool classof(const MCSection *S) { return S->getVariant() == SV_GBZ80; }

private:
  GBZ80SectionData Data;

  
};

/// Lowering for an GBZ80 object file.
class GBZ80TargetObjectFile : public TargetLoweringObjectFile {
  typedef TargetLoweringObjectFile Base;

public:
  void Initialize(MCContext &ctx, const TargetMachine &TM) override;

  MCSection *SelectSectionForGlobal(const GlobalObject *GO, SectionKind Kind,
                                    const TargetMachine &TM) const override;
  MCSection *getSectionForConstant(const DataLayout &DL, SectionKind Kind,
                                   const Constant *C, unsigned &Align) const;
  // Not actually const.
  MCSectionGBZ80 *getSection(SectionKind Kind, StringRef N, GBSectionType T,
    uint16_t A, unsigned B, unsigned Align, const GlobalObject *GO) const;

private:
  // Making this mutable is disgusting, but I want to avoid having to mess
  // around in MCContext and friends.
  // TODO[str4d]: Does this need to change now that MCSection stores the section name?
  mutable std::map<GBZ80SectionData, MCSectionGBZ80 *> Sections;


  virtual MCSection * getExplicitSectionGlobal(const GlobalObject * GO,
    SectionKind Kind, const TargetMachine & TM) const override;

};

} // end namespace llvm

#endif // LLVM_GBZ80_TARGET_OBJECT_FILE_H
