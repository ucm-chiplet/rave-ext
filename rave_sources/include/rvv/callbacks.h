/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/
#pragma once
#include "state.h"
#include "instr_data.h"
#include "scalar_blocks.h"
void insn_exec(thread_state_t * state, instr_data * instr);

void rolling_scalar_block(uint32_t opcode, uint64_t PC, scalar_block_data_t * data);

void scalar_block_exec(thread_state_t * state, scalar_block_data_t * data);
