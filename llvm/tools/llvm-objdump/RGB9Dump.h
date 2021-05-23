//===-- RGB9Dump.h - RGB9-specific dumper -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TOOLS_LLVM_OBJDUMP_RGB9DUMP_H
#define LLVM_TOOLS_LLVM_OBJDUMP_RGB9DUMP_H

#include "llvm/ADT/SmallVector.h"

namespace llvm {

class Error;

namespace object {
class RGB9ObjectFile;
class ObjectFile;
class RelocationRef;
} // namespace object

namespace objdump {

Error getRGB9RelocationValueString(const object::RGB9ObjectFile *Obj,
                                   const object::RelocationRef &RelRef,
                                   llvm::SmallVectorImpl<char> &Result);

void printRGB9FileHeader(const object::ObjectFile *O);

} // namespace objdump
} // namespace llvm

#endif
