//===-- GBZ80MCAsmInfo.cpp - GBZ80 asm properties -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the GBZ80MCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "GBZ80MCAsmInfo.h"

#include "llvm/ADT/Triple.h"

namespace llvm {

GBZ80MCAsmInfo::GBZ80MCAsmInfo(const Triple &TT, const MCTargetOptions &Options) {
  CodePointerSize = 2;
  CalleeSaveStackSlotSize = 2;
  MaxInstLength = 3;
  CommentString = ";";
  PrivateGlobalPrefix = "L";
  PrivateLabelPrefix = ".L";
  OctalPrefix = "&";
  HexadecimalPrefix = "$";
  ZeroDirective = "DS ";
  AsciiDirective = nullptr; // Can't use this because of hardcoded strings.
  AscizDirective = nullptr; // DB does not zero-terminate.
  ByteListDirective = "DB ";
  Data8bitsDirective = "DB ";
  Data16bitsDirective = "DW ";
  Data32bitsDirective = "DL ";
  P2Align8bitsDirective = "align ";
  P2Align16bitsDirective = "align "; // align doesn't have a fill_value parameter.
  P2Align32bitsDirective = "align "; // align doesn't have a fill_value parameter.
  GlobalDirective = "EXPORT ";
  HasDotTypeDotSizeDirective = false;
  HasSingleParameterDotFile = false;
  WeakDirective = nullptr;
  SupportsDebugInformation = false;
  UseIntegratedAssembler = false;
}

} // end of namespace llvm
