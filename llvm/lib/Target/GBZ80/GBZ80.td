//===-- GBZ80.td - Describe the GBZ80 Target Machine ----------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===---------------------------------------------------------------------===//
// This is the top level entry point for the GBZ80 target.
//===---------------------------------------------------------------------===//

//===---------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===---------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===---------------------------------------------------------------------===//
// GBZ80 Device Definitions
//===---------------------------------------------------------------------===//

include "GBZ80Devices.td"

//===---------------------------------------------------------------------===//
// Register File Description
//===---------------------------------------------------------------------===//

include "GBZ80RegisterInfo.td"

//===---------------------------------------------------------------------===//
// Instruction Descriptions
//===---------------------------------------------------------------------===//

include "GBZ80InstrInfo.td"

def GBZ80InstrInfo : InstrInfo;

//===---------------------------------------------------------------------===//
// Calling Conventions
//===---------------------------------------------------------------------===//

include "GBZ80CallingConv.td"

//===---------------------------------------------------------------------===//
// Assembly Printers
//===---------------------------------------------------------------------===//

def GBZ80AsmWriter : AsmWriter {
 string AsmWriterClassName = "InstPrinter";
 bit isMCAsmWriter = 1;
}

//===---------------------------------------------------------------------===//
// Target Declaration
//===---------------------------------------------------------------------===//

def GBZ80 : Target {
  let InstructionSet         = GBZ80InstrInfo;
  let AssemblyWriters        = [GBZ80AsmWriter];
}

