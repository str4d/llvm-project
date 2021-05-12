//===--------- GBZ80PreEmit.cpp - Post ISel pass -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a pass that runs after instruction selection.
//
//===----------------------------------------------------------------------===//

#include "GBZ80.h"
#include "GBZ80InstrInfo.h"
#include "GBZ80TargetMachine.h"
#include "MCTargetDesc/GBZ80MCTargetDesc.h"

#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

using namespace llvm;

#define PASS_NAME "GBZ80 pre emit"
#define DEBUG_TYPE "gbz80-pre-emit"

namespace {

class GBZ80PreEmit : public MachineFunctionPass {
public:
  static char ID;

  GBZ80PreEmit() : MachineFunctionPass(ID) {
    initializeGBZ80PreEmitPass(*PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  StringRef getPassName() const override { return PASS_NAME; }

private:
  typedef MachineBasicBlock Block;
  typedef Block::iterator BlockIt;

  const GBZ80RegisterInfo *TRI;
  const TargetInstrInfo *TII;
  MachineRegisterInfo *MRI;

};

char GBZ80PreEmit::ID = 0;


bool GBZ80PreEmit::runOnMachineFunction(MachineFunction &MF) {
  bool Modified = false;

  const GBZ80Subtarget &STI = MF.getSubtarget<GBZ80Subtarget>();
  TRI = STI.getRegisterInfo();
  TII = STI.getInstrInfo();

  return Modified;
}

} // end of anonymous namespace

INITIALIZE_PASS(GBZ80PreEmit, DEBUG_TYPE,
                PASS_NAME, false, false)
namespace llvm {

FunctionPass *createGBZ80PreEmitPass() { return new GBZ80PreEmit(); }

} // end of namespace llvm
