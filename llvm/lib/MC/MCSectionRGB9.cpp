//===- lib/MC/MCSectionRGB9.cpp - RGB9 Code Section Representation --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/MCSectionRGB9.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/MathExtras.h"

using namespace llvm;

void RGB9SectionData::getAsString(SmallVectorImpl<char> &Str) const {
  raw_svector_ostream O(Str);
  switch (Type) {
  case RGB9::SEC_ROM0:  O << "ROM0"; break;
  case RGB9::SEC_ROMX:  O << "ROMX"; break;
  case RGB9::SEC_VRAM:  O << "VRAM"; break;
  case RGB9::SEC_SRAM:  O << "SRAM"; break;
  case RGB9::SEC_WRAM0: O << "WRAM0"; break;
  case RGB9::SEC_WRAMX: O << "WRAMX"; break;
  case RGB9::SEC_OAM:   O << "OAM"; break;
  case RGB9::SEC_HRAM:  O << "HRAM"; break;
  default: llvm_unreachable("Tried to print an invalid section!");
  }
  if (hasAddress())
    O << "[$" << format("%04x", Address) << "]";
  if (hasBank())
    O << ",BANK[" << Bank << "]";
}

void MCSectionRGB9::PrintSwitchToSection(const MCAsmInfo &MAI, const Triple &T,
                                         raw_ostream &OS,
                                         const MCExpr *Subsection) const {
  // XXX: this will break if sectionname has quotes in it.
  StringRef SectionName = getName();
  if (SectionName.empty())
    OS << "SECTION @,";
  else
    OS << "SECTION \"" << SectionName << "\",";

  // This is just as simple as asking the section data for its string and
  // printing it.
  SmallString<40> SecStr;
  Data.getAsString(SecStr);
  OS << SecStr;

  auto Alignment = getAlignment();
  if (Alignment > 1) {
    auto align = Log2_32_Ceil(Alignment);
    OS << ",ALIGN[" << align;
    // getAlignment() currently always returns a power of 2, but for spec accuracy:
    auto offset = Alignment % (1 << align);
    if (offset > 0) {
      OS << ", " << offset;
    }
    OS << "]";
  }

  OS << '\n';
}

bool MCSectionRGB9::UseCodeAlign() const {
  // RGBDS has support for alignment.
  return false;
}

bool MCSectionRGB9::isVirtualSection() const {
  // I don't believe RGBDS has any of these.
  // XXX: We might want a fake section I guess? ST_NONE?
  return false;
}
