//===-- MCRGB9ObjectTargetWriter.cpp - RGB9 Target Writer Subclass --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/MCRGB9ObjectWriter.h"

using namespace llvm;

MCRGB9ObjectTargetWriter::MCRGB9ObjectTargetWriter() {}

// Pin the vtable to this object file
MCRGB9ObjectTargetWriter::~MCRGB9ObjectTargetWriter() = default;
