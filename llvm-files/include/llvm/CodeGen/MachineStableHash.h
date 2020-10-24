//===------------ MIRVRegNamerUtils.h - MIR VReg Renaming Utilities -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Stable hashing for MachineInstr and MachineOperand. Useful or getting a
// hash across runs, modules, etc.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CODEGEN_MACHINESTABLEHASH_H
#define LLVM_CODEGEN_MACHINESTABLEHASH_H

#include "llvm/CodeGen/StableHashing.h"

namespace llvm {
class MachineInstr;
class MachineOperand;

stable_hash stableHashValue(const MachineOperand &MO);
stable_hash stableHashValue(const MachineInstr &MI, bool HashVRegs = false,
                            bool HashConstantPoolIndices = false,
                            bool HashMemOperands = false);

} // namespace llvm

#endif
