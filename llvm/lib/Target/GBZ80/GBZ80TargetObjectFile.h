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
#include "llvm/MC/MCSectionRGB9.h"
#include "llvm/Target/TargetLoweringObjectFile.h"

#include <map>

namespace llvm {

/// Lowering for a GBZ80 RGB9 object file.
class GBZ80TargetObjectFile : public TargetLoweringObjectFile {
  typedef TargetLoweringObjectFile Base;

public:
  void Initialize(MCContext &ctx, const TargetMachine &TM) override;

  MCSection *SelectSectionForGlobal(const GlobalObject *GO, SectionKind Kind,
                                    const TargetMachine &TM) const override;
  MCSection *getSectionForConstant(const DataLayout &DL, SectionKind Kind,
                                   const Constant *C, unsigned &Align) const;
  // Not actually const.
  MCSectionRGB9 *getSection(SectionKind Kind, StringRef N, GBSectionType T,
    uint16_t A, unsigned B, unsigned Align, const GlobalObject *GO) const;

private:
  // Making this mutable is disgusting, but I want to avoid having to mess
  // around in MCContext and friends.
  // TODO[str4d]: Does this need to change now that MCSection stores the section name?
  mutable std::map<RGB9SectionData, MCSectionRGB9 *> Sections;


  virtual MCSection * getExplicitSectionGlobal(const GlobalObject * GO,
    SectionKind Kind, const TargetMachine & TM) const override;

};

} // end namespace llvm

#endif // LLVM_GBZ80_TARGET_OBJECT_FILE_H
