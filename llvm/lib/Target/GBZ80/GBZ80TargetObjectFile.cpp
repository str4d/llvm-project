//===-- GBZ80TargetObjectFile.cpp - GBZ80 Object Files ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "GBZ80TargetObjectFile.h"

#include "llvm/BinaryFormat/ELF.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionRGB9.h"

#include "GBZ80.h"

namespace llvm {

void GBZ80TargetObjectFile::Initialize(MCContext &Ctx, const TargetMachine &TM) {
  Base::Initialize(Ctx, TM);
  // Make some fake generic sections.
  TextSection = getSection(SectionKind::getText(), StringRef(".text"),
      GBSectionType::ST_ROM0, 0, ~0U, 1, nullptr);
  DataSection = getSection(SectionKind::getText(), StringRef(".data"),
    GBSectionType::ST_WRAM0, 0, ~0U, 1, nullptr);
  BSSSection = getSection(SectionKind::getText(), StringRef(".bss"),
    GBSectionType::ST_WRAM0, 0, ~0U, 1, nullptr);
  ReadOnlySection = getSection(SectionKind::getText(), StringRef(".readonly"),
    GBSectionType::ST_ROM0, 0, ~0U, 1, nullptr);
}

MCSectionRGB9 *GBZ80TargetObjectFile::getSection(SectionKind Kind,
    StringRef N, GBSectionType T, uint16_t A, unsigned B, unsigned Align,
    const GlobalObject *GO) const {
  assert((!N.empty() || GO) && "Named unique section?");
  RGB9SectionData Data(T, A, B, Align, GO);
  if (Sections.count(Data))
    return Sections[Data];
  // Not allocated with an allocator...
  MCSectionRGB9 *Section = new MCSectionRGB9(N, Kind, Data, nullptr);
  Sections[Data] = Section;
  return Section;
}

MCSection *
GBZ80TargetObjectFile::SelectSectionForGlobal(const GlobalObject *GO,
                                              SectionKind Kind,
                                              const TargetMachine &TM) const {
  // This only gets called for globals that don't have a 'section' attribute.
  // XXX: Could they have custom attributes though? Perhaps only makes sense
  // for constants.
  GBSectionType Type;
  // We need to enforce sections here.
  // Should this verify the section on the actual object?
  if (Kind.isText() || Kind.isReadOnly()) {
    // Code and constants. This goes in ROM0 or ROMX, always. It is an error to
    // put it in any other section. If no section is given, it goes in ROM0.
    // XXX: Make it ROM0 for now.
    Type = ST_ROM0;
  } else if (Kind.isBSS() || Kind.isData()) {
    // This is writeable data.
    // Unsure how to handle this, since it needs to be inited.
    // Special section perhaps? 
    // This can go in any section except for ROM0 and ROMX. Default is WRAM0.
    // XXX: WRAM0 for now.
    Type = ST_WRAM0;
  }

  return getSection(Kind, (Twine(GO->getParent()->getName()) + "_" + GO->getName()).str(),
                    Type, 0, ~0U, GO->getAlignment(), GO);
}
MCSection* 
GBZ80TargetObjectFile::getSectionForConstant(const DataLayout &DL,
    SectionKind Kind, const Constant *C, unsigned &Align) const {
  // This might actually be enough. Any explicit section will end up in
  // getExplicitSectionGlobal.
  // XXX: unless we want to specify deduced constant sections with attrs?
  // XXX: We can't specify a symbol for this unique section. is this a problem?
  assert(Kind.isReadOnly() && "Writeable constant?");
  return getSection(Kind, StringRef(), ST_ROM0, 0, ~0U, Align, nullptr);
}

MCSection*
GBZ80TargetObjectFile::getExplicitSectionGlobal(const GlobalObject * GO,
    SectionKind Kind, const TargetMachine & TM) const {
  // This is supposed to take the explicit section string on the GO and return
  // an MCSection for it. Decide on a format for this.
  // XXX: Is this aware of the subtarget? Could this verify the section?
  llvm_unreachable("getExplicitSectionGlobal not yet implemented!");
}

} // end of namespace llvm

