//===-- llvm/MC/MCRGB9ObjectWriter.h - RGB9 Object Writer -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_MC_MCRGB9OBJECTWRITER_H
#define LLVM_MC_MCRGB9OBJECTWRITER_H

#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>

namespace llvm {

class MCRGB9ObjectTargetWriter : public MCObjectTargetWriter {
protected:
  MCRGB9ObjectTargetWriter();

public:
  virtual ~MCRGB9ObjectTargetWriter();

  Triple::ObjectFormatType getFormat() const override { return Triple::RGB9; }
  static bool classof(const MCObjectTargetWriter *W) {
    return W->getFormat() == Triple::RGB9;
  }
};

/// Construct a new RGB9 writer instance.
///
/// \param MOTW - The target specific RGB9 writer subclass.
/// \param OS - The stream to write to.
/// \returns The constructed object writer.
std::unique_ptr<MCObjectWriter>
createRGB9ObjectWriter(std::unique_ptr<MCRGB9ObjectTargetWriter> MOTW,
                       raw_pwrite_stream &OS);

} // namespace llvm

#endif // LLVM_MC_MCRGB9OBJECTWRITER_H
