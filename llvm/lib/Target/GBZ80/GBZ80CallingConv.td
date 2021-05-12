//===-- GBZ80CallingConv.td - Calling Conventions for GBZ80 ----*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for GBZ80 architecture.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// GBZ80 Return Value Calling Convention
//===----------------------------------------------------------------------===//

def RetCC_GBZ80 : CallingConv
<[
  // i8 is returned in B.
  CCIfType<[i8], CCAssignToReg<[RA]>>,

  // i16 are returned in BC.
  CCIfType<[i16], CCAssignToReg<[RHL]>>
]>;

//===----------------------------------------------------------------------===//
// GBZ80 Argument Calling Conventions
//===----------------------------------------------------------------------===//

/// Allocate:
/// *-----*-------*-------*-------*-----------*
/// | i8  |  A    |  E    |  C    | stack ... |
/// *-----*-------*-------*-------*-----------*
/// | i16 |  HL   |  DE   |  BC   | stack ... |
/// *-----*-------*-------*-------*-----------*
/// A and HL shadow each other.
/// TODO: I suspect it's a bad idea to actually use HL in the calling
/// convention. It means it will be really difficult to load values
/// during the call sequence if they need to be loaded from stack slots.
def ArgCC_GBZ80 : CallingConv
<[
  CCIfNotVarArg<
    CCIfType<[i8],
      CCAssignToRegWithShadow<[RA], [RHL]>
    >
  >,
  CCIfNotVarArg<
    CCIfType<[i16],
      CCAssignToRegWithShadow<[RHL], [RA]>
    >
  >,
  CCIfNotVarArg<
    CCIfType<[i8],
      CCAssignToReg<[RE, RC]>
    >
  >,
  CCIfNotVarArg<
    CCIfType<[i16],
      CCAssignToReg<[RDE, RBC]>
    >
  >,
  CCAssignToStack<2, 1>
]>;

// Calling convention for variadic functions.
// TODO: Keep this convention?
def ArgCC_GBZ80_Vararg : CallingConv
<[
  // i16 are always passed through the stack with an alignment of 1.
  CCIfType<[i8], CCPromoteToType<i16>>,
  CCAssignToStack<2, 1>
]>;

//===----------------------------------------------------------------------===//
// Callee-saved register lists.
//===----------------------------------------------------------------------===//

def CSR_0_1        : CalleeSavedRegs<(add RDE, RBC)>;
def CSR_2          : CalleeSavedRegs<(add RBC)>;
def CSR_Interrupts : CalleeSavedRegs<(add RAF, RBC, RDE, RHL)>;
