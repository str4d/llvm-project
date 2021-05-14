//===-- GBZ80AsmPrinter.cpp - GBZ80 LLVM assembly writer ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format GBZ80 assembly language.
//
//===----------------------------------------------------------------------===//

#include "GBZ80.h"
#include "GBZ80MCInstLower.h"
#include "GBZ80Subtarget.h"
#include "MCTargetDesc/GBZ80InstPrinter.h"

#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

#define DEBUG_TYPE "GBZ80-asm-printer"

namespace llvm {

/// An GBZ80 assembly code printer.
class GBZ80AsmPrinter : public AsmPrinter {
public:
  GBZ80AsmPrinter(TargetMachine &TM,
                std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)), MRI(*TM.getMCRegisterInfo()) { }

  StringRef getPassName() const override { return "GBZ80 Assembly Printer"; }

  void printOperand(const MachineInstr *MI, unsigned OpNo, raw_ostream &O);

  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNum,
                       const char *ExtraCode, raw_ostream &O) override;

  bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNum,
                             const char *ExtraCode, raw_ostream &O) override;

  void emitFunctionBodyStart() override;

  void emitInstruction(const MachineInstr *MI) override;

  void emitLinkage(const GlobalValue *GV, MCSymbol *GVSym) const;

private:
  const MCRegisterInfo &MRI;
};

void GBZ80AsmPrinter::printOperand(const MachineInstr *MI, unsigned OpNo,
                                 raw_ostream &O) {
  const MachineOperand &MO = MI->getOperand(OpNo);

  switch (MO.getType()) {
  case MachineOperand::MO_Register:
    O << GBZ80InstPrinter::getPrettyRegisterName(OpNo);
    break;
  case MachineOperand::MO_Immediate:
    O << MO.getImm();
    break;
  case MachineOperand::MO_GlobalAddress:
    O << getSymbol(MO.getGlobal());
    break;
  case MachineOperand::MO_ExternalSymbol:
    O << *GetExternalSymbolSymbol(MO.getSymbolName());
    break;
  case MachineOperand::MO_MachineBasicBlock:
    O << *MO.getMBB()->getSymbol();
    break;
  default:
    llvm_unreachable("Not implemented yet!");
  }
}

bool GBZ80AsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNum,
                                    const char *ExtraCode, raw_ostream &O) {
  // Default asm printer can only deal with some extra codes,
  // so try it first.
  bool Error = AsmPrinter::PrintAsmOperand(MI, OpNum, ExtraCode, O);
  if (Error)
    printOperand(MI, OpNum, O);

  return false;
}

bool GBZ80AsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
                                          unsigned OpNum, const char *ExtraCode,
                                          raw_ostream &O) {
#if 0
  if (ExtraCode && ExtraCode[0]) {
    llvm_unreachable("This branch is not implemented yet");
  }

  const MachineOperand &MO = MI->getOperand(OpNum);
  (void)MO;
  assert(MO.isReg() && "Unexpected inline asm memory operand");

  // TODO: We should be able to look up the alternative name for
  // the register if it's given.
  // TableGen doesn't expose a way of getting retrieving names
  // for registers.
  if (MI->getOperand(OpNum).getReg() == GB::R31R30) {
    O << "Z";
  } else {
    assert(MI->getOperand(OpNum).getReg() == GB::R29R28 &&
           "Wrong register class for memory operand.");
    O << "Y";
  }

  // If NumOpRegs == 2, then we assume it is product of a FrameIndex expansion
  // and the second operand is an Imm.
  unsigned OpFlags = MI->getOperand(OpNum - 1).getImm();
  unsigned NumOpRegs = InlineAsm::getNumOperandRegisters(OpFlags);

  if (NumOpRegs == 2) {
    O << '+' << MI->getOperand(OpNum + 1).getImm();
  }
#endif
  return false;
}

void GBZ80AsmPrinter::emitFunctionBodyStart() {
  const MachineRegisterInfo &MRI = MF->getRegInfo();

  SmallString<128> C;
  raw_svector_ostream O(C);
  O << " ===== Function " << MF->getName() << " =====";
  OutStreamer->emitRawComment(C, false);
  C.clear();
  O << " Arguments: ";
  if (!MRI.livein_empty()) {
    for (MachineRegisterInfo::livein_iterator
      I = MRI.livein_begin(), E = MRI.livein_end(); I != E; ++I) {
      O << printReg(I->first, MRI.getTargetRegisterInfo());
      if (std::next(I) != E)
        O << ", ";
    }
  }
  OutStreamer->emitRawComment(C, false);
  C.clear();
  O << " Returns:   TODO";
  OutStreamer->emitRawComment(C, false);
  C.clear();
  O << " =====================";
  for (unsigned i = 0; i < MF->getName().size(); i++)
    O << "=";
  OutStreamer->emitRawComment(C, false);
}

void GBZ80AsmPrinter::emitInstruction(const MachineInstr *MI) {
  GBZ80MCInstLower MCInstLowering(OutContext, *this);

  MCInst I;
  MCInstLowering.lowerInstruction(*MI, I);
  EmitToStreamer(*OutStreamer, I);
}

void GBZ80AsmPrinter::emitLinkage(const GlobalValue *GV,
    MCSymbol *GVSym) const {
  GlobalValue::LinkageTypes Linkage = GV->getLinkage();
  switch (Linkage) {
  case GlobalValue::ExternalLinkage:
    OutStreamer->emitSymbolAttribute(GVSym, MCSA_Global);
    break;
  case GlobalValue::PrivateLinkage:
  case GlobalValue::InternalLinkage:
    // Emit nothing for private and internal.
    break;
  case GlobalValue::CommonLinkage:
  case GlobalValue::LinkOnceAnyLinkage:
  case GlobalValue::LinkOnceODRLinkage:
  case GlobalValue::WeakAnyLinkage:
  case GlobalValue::WeakODRLinkage:
    llvm_unreachable("Unsupported linkage types!");
  case GlobalValue::AppendingLinkage:
  case GlobalValue::AvailableExternallyLinkage:
  case GlobalValue::ExternalWeakLinkage:
    llvm_unreachable("Should never emit this");
  }
}

} // end of namespace llvm

extern "C" void LLVMInitializeGBZ80AsmPrinter() {
  llvm::RegisterAsmPrinter<llvm::GBZ80AsmPrinter> X(llvm::getTheGBZ80Target());
}

