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
  case ST_ROM0:  O << "ROM0"; break;
  case ST_ROMX:  O << "ROMX"; break;
  case ST_VRAM:  O << "VRAM"; break;
  case ST_SRAM:  O << "SRAM"; break;
  case ST_WRAM0: O << "WRAM0"; break;
  case ST_WRAMX: O << "WRAMX"; break;
  case ST_OAM:   O << "OAM"; break;
  case ST_HRAM:  O << "HRAM"; break;
  default: llvm_unreachable("Tried to print an invalid section!");
  }
  if (hasAddress())
    O << "[$" << format("%04x", Address) << "]";
  if (hasBank())
    O << ",BANK[" << Bank << "]";
  // Round alignment up to the nearest log2. This will guarantee at least that
  // alignment. I don't think non-power-of-two alignments are legal anyway.
  if (hasAlignment())
    O << ",ALIGN[" << Log2_32_Ceil(Alignment) << "]";
}

bool RGB9SectionData::get(StringRef Specifier, RGB9SectionData &Data) {
  // Set defaults.
  Data.Type = ST_NONE;
  Data.Address = 0;
  Data.Bank = ~0U;
  Data.Alignment = 1;

  // TODO: I guess this isn't needed just yet.
  return false;
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
  OS << SecStr << '\n';
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
