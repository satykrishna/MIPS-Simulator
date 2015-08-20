//
// This file contains functions to simulate instruction
// execution pipeline using Tomasulo Algorithm
//

#include "FuncProto.h"

bool getAvailableROBEntry(MipsSimContext* pContext, UINT* ROBEntry)
{
    UINT count = 0;

    for (count = 0; count < MAX_ROB_ENTRIES; count++)
    {
        if (!pContext->ROB[count].Busy)
        {
            *ROBEntry = count;
            return true;
        }
    }

    return false;
}

bool getAvailableRSEntry(MipsSimContext* pContext, UINT* RSEntry)
{
    UINT count = 0;

    for (count = 0; count < MAX_RS_ENTRIES; count++)
    {
        if (!pContext->RS[count].Busy)
        {
            *RSEntry = count;
            return true;
        }
    }

    return false;
}

bool IsSimComplete(MipsSimContext *pContext)
{
    UINT        count = 0;

    // Simulation will be complete when Instruction Queue is empty and all the ROB entries are free 
    for (count = 0; count < MAX_ROB_ENTRIES; count++)
    {
        if (pContext->ROB[count].Busy)
        {
            return false;
        }
    }

    // All the ROB entries are free, check for IQ 
    if (!IsInstQueueEmpty(pContext))
    {
        // IQ not empty;
        return false;
    }

    return true;
}

void fetchInst(MipsSimContext *pContext)
{
    Instruction *pInst = NULL;

    // For the instructions already in the IQ, move them to IS for this cycle
    moveInstToISStage(pContext);

    // Get the current Instruction from the PC 
    pInst = getInstFromList(pContext, pContext->CurrentPC);
    if (pInst)
    {
        pInst->InstIssueNum = pContext->CurrentInstIssueNum++;

        // Increment PC 
        pContext->CurrentPC = getNextPCAddress(pContext, pContext->CurrentPC);

        // Add the Inst to IQ 
        addInstToInstQueue(pContext, pInst);
    }
}

void issueInst(MipsSimContext* pContext)
{
    Instruction *pInst      = NULL;
    UINT        ROBEntry    = 0;
    UINT        RSEntry     = 0;
    BTBEntry    *pBTBEntry  = NULL;

    // Check if there is an instruction to be issued from IQ 
    pInst = getInstToIssue(pContext);

    // BREAK and NOP Instructions enter ROB to be ready to be commited in next cycle, dont enter RS
    if (pInst && pInst->IsBreakOrNopInst && getAvailableROBEntry(pContext, &ROBEntry))
    {
        // ROB available, remove instruction from Inst Queue
        removeInstFromInstQueue(pContext, pInst);

        // Add the instruction to ROB!
        pContext->ROB[ROBEntry].pInst = pInst;
        pContext->ROB[ROBEntry].Ready = true;
        pContext->ROB[ROBEntry].Busy = true;
        pContext->ROB[ROBEntry].Value = 0;

        // Instruction is ready to be committed in next cycle 
        pInst->UpdatePipelineStage = true;
    }
    else
    {
        // Check if there is a empty ROB and entry and empty RS entry to issue this instruction
        // Get an empty ROB Entry for the Instruction 
        if (pInst && getAvailableROBEntry(pContext, &ROBEntry) && getAvailableRSEntry(pContext, &RSEntry))
        {
            // Both ROB and RS available!!
            // Remove the Instruction from IQ
            removeInstFromInstQueue(pContext, pInst);

            // Bookkeeping for all instructions that have rs operand 
            if (pInst->IsRsUsedAsOperand)
            {
                if (pContext->RegisterStat[pInst->InstructionDWord.RType.Rs].Busy)
                {
                    // Register value not available, some in flight Instrcution will write to this register
                    // check if the in flight instrution is in Ready State in the ROB
                    if (pContext->ROB[pContext->RegisterStat[pInst->InstructionDWord.RType.Rs].Reorder].Ready)
                    {
                        // Instruction is in ready state, get the value from the ROB, update RS 
                        pContext->RS[RSEntry].Vj = pContext->ROB[pContext->RegisterStat[pInst->InstructionDWord.RType.Rs].Reorder].Value;
                        pContext->RS[RSEntry].Qj = MAX_ROB_ENTRIES;
                    }
                    else
                    {
                        // Wait for the instruction to complete, update RS with the ROB Entry to wait on
                        pContext->RS[RSEntry].Qj = pContext->RegisterStat[pInst->InstructionDWord.RType.Rs].Reorder;
                    }
                }
                else
                {
                    // rs Register value is available, Update Qj and Vj in the Reservation Station
                    pContext->RS[RSEntry].Vj = RegLookUpTable[pInst->InstructionDWord.RType.Rs].RegValue;
                    pContext->RS[RSEntry].Qj = MAX_ROB_ENTRIES;
                }
            }

            // Book keeping for all Instructions that have Rt as an operand 
            if (pInst->IsRtUsedAsOperand)
            {
                if (pContext->RegisterStat[pInst->InstructionDWord.RType.Rt].Busy)
                {
                    // Register value not available, some in flight Instrcution will write to this register
                    // check if the in flight instrution is in Ready State in the ROB
                    if (pContext->ROB[pContext->RegisterStat[pInst->InstructionDWord.RType.Rt].Reorder].Ready)
                    {
                        // Instruction is in ready state, get the value from the ROB, update RS 
                        pContext->RS[RSEntry].Vk = pContext->ROB[pContext->RegisterStat[pInst->InstructionDWord.RType.Rt].Reorder].Value;
                        pContext->RS[RSEntry].Qk = MAX_ROB_ENTRIES;
                    }
                    else
                    {
                        // Wait for the instruntion to complete, update RS with the ROB Entry to wait on
                        pContext->RS[RSEntry].Qk = pContext->RegisterStat[pInst->InstructionDWord.RType.Rt].Reorder;
                    }
                }
                else
                {
                    // rs Register value is available, Update Qj and Vj in the Reservation Station
                    pContext->RS[RSEntry].Vk = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegValue;
                    pContext->RS[RSEntry].Qk = MAX_ROB_ENTRIES;
                }
            }
            
            // For ADDI, ADDIU & SLTI, imm value is added to Vk
            if (pInst->IsImmALUInst)
            {
                pContext->RS[RSEntry].Vk = pInst->InstructionDWord.IType.SignedImmediate;
                pContext->RS[RSEntry].Qk = MAX_ROB_ENTRIES;
            }

            // For SRL, SLL and SRA, shift amount is added to Vk
            if (pInst->IsShiftALUInst)
            {
                pContext->RS[RSEntry].Vk = pInst->InstructionDWord.RType.Shamt;
                pContext->RS[RSEntry].Qk = MAX_ROB_ENTRIES;
            }

            // Update the Register Stat, RS & ROB depending upon which register is used as destination
            pContext->ROB[ROBEntry].pInst = pInst;
            pContext->RS[RSEntry].pInst = pInst;
            pContext->ROB[ROBEntry].Ready = false;
            pContext->ROB[ROBEntry].Busy = true;
            pContext->RS[RSEntry].Busy = true;
            pContext->RS[RSEntry].Dest = ROBEntry;
            
            // Move the instruction to next stage for Execution
            pInst->UpdatePipelineStage = true;

            if (pInst->IsRdUsedAsDest)
            {
                pContext->RegisterStat[pInst->InstructionDWord.RType.Rd].Reorder = ROBEntry;
                pContext->RegisterStat[pInst->InstructionDWord.RType.Rd].Busy = true;
                pContext->ROB[ROBEntry].Dest = pInst->InstructionDWord.RType.Rd;
            }

            if (pInst->IsRtUsedAsDest)
            {
                pContext->RegisterStat[pInst->InstructionDWord.RType.Rt].Reorder = ROBEntry;
                pContext->RegisterStat[pInst->InstructionDWord.RType.Rt].Busy = true;
                pContext->ROB[ROBEntry].Dest = pInst->InstructionDWord.RType.Rt;
            }

            // Only for Load/Store and Branch Instructions
            // Add imm value to address field
            if (pInst->IsLoadOrStoreInst)
            {
                pContext->RS[RSEntry].Addr = pInst->InstructionDWord.IType.SignedImmediate;
            }

            if (pInst->IsBranchInst)
            {
                pContext->RS[RSEntry].Addr = pContext->RS[RSEntry].pInst->InstAddress + 4 + pInst->InstructionDWord.IType.SignedImmediate*4;
            }

            // Jump Instruction does enter the ALU RS, even though no computation is required 
            if (pInst->IsJumpInst)
            {
                pContext->RS[RSEntry].Addr = pInst->InstructionDWord.JType.Target;
            }
        }
    }
}

void executeInst(MipsSimContext *pContext)
{
    UINT        CurrRSEntry = 0;
    Instruction *pInst = NULL;
    UINT        count = 0;
    bool        ComputeAddress = true;
    UINT        ROBCount = 0;
    UINT        RSCount = 0;

    // Check if there is an instruction ready in RS to execute
    for (CurrRSEntry = 0; CurrRSEntry < MAX_RS_ENTRIES; CurrRSEntry++)
    {
        if (pContext->RS[CurrRSEntry].Busy && pContext->RS[CurrRSEntry].pInst->PipelineStage == INSTRUCTION_EX)
        {
            pInst = pContext->RS[CurrRSEntry].pInst;

            // To check whehter the instruction can start execution
            // NOP and BREAK Instructions will not enter this stage
            // For Jump Instruction start execution, operand will be available
            // if both Rs & Rt were operands and both are available, then start 
            // if Rs is the only operand, then check for only Qj
            // if Rt is the only operand, then check for only Qk 
            if ((pInst->IsRsUsedAsOperand && pInst->IsRtUsedAsOperand && 
                pContext->RS[CurrRSEntry].Qj == MAX_ROB_ENTRIES && pContext->RS[CurrRSEntry].Qk == MAX_ROB_ENTRIES) ||
                (pInst->IsRsUsedAsOperand && !pInst->IsRtUsedAsOperand && pContext->RS[CurrRSEntry].Qj == MAX_ROB_ENTRIES) ||
                (!pInst->IsRsUsedAsOperand && pInst->IsRtUsedAsOperand && pContext->RS[CurrRSEntry].Qk == MAX_ROB_ENTRIES) ||
                pInst->IsJumpInst)
            {
                // Operands availble, EXECUTE !!!!!
                switch (pInst->Opcode)
                {
                case OPCODE_SPECIAL:
                    switch (pInst->InstructionDWord.RType.Function)
                    {
                    case FUNC_SRL:
                    case FUNC_SRA:
                        pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj >> pContext->RS[CurrRSEntry].Vk;
                        break;

                    case FUNC_SLL:
                        pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj << pContext->RS[CurrRSEntry].Vk;
                        break;

                    case FUNC_ADD:
                    case FUNC_ADDU:
                        pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj + pContext->RS[CurrRSEntry].Vk;
                        break;

                    case FUNC_SUB:
                    case FUNC_SUBU:
                        pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj - pContext->RS[CurrRSEntry].Vk;
                        break;

                    case FUNC_AND:
                        pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj & pContext->RS[CurrRSEntry].Vk;
                        break;

                    case FUNC_OR:
                        pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj | pContext->RS[CurrRSEntry].Vk;
                        break;

                    case FUNC_XOR:
                        pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj ^ pContext->RS[CurrRSEntry].Vk;
                        break;

                    case FUNC_NOR:
                        pContext->RS[CurrRSEntry].Result = ~(pContext->RS[CurrRSEntry].Vj | pContext->RS[CurrRSEntry].Vk);
                        break;

                    case FUNC_SLT:
                    case FUNC_SLTU:
                        pContext->RS[CurrRSEntry].Result = (pContext->RS[CurrRSEntry].Vj < pContext->RS[CurrRSEntry].Vk) ? 1 : 0;
                        break;

                    default:
                        // should not reach here! 
                        break;
                    }
                    break;

                case OPCODE_REGIMM:
                    switch (pInst->InstructionDWord.IType.Rt)
                    {
                    case RT_BLTZ:
                        if (pContext->RS[CurrRSEntry].Vj < 0)
                        {
                            // Branch Taken 
                            pInst->BranchOutcome = BRANCH_TAKEN;
                        }
                        else
                        {
                            // Branch Not Taken
                            pInst->BranchOutcome = BRANCH_NOT_TAKEN;
                        }

                        // Mark the instruction to be ready
                        pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                        break;

                    case RT_BGEZ:
                        if (pContext->RS[CurrRSEntry].Vj >= 0)
                        {
                            // Branch Taken 
                            pInst->BranchOutcome = BRANCH_TAKEN;
                        }
                        else
                        {
                            // Branch Not Taken
                            pInst->BranchOutcome = BRANCH_NOT_TAKEN;
                        }

                        // Mark the instruction to be ready
                        pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                        break;

                    default:
                        // shoudld not reach here!
                        break;
                    }

                case OPCODE_J:
                    // Branch Taken 
                    pInst->BranchOutcome = BRANCH_TAKEN;

                    // Mark the instruction to be ready
                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                    break;

                case OPCODE_BEQ:
                    if (pContext->RS[CurrRSEntry].Vj == pContext->RS[CurrRSEntry].Vk)
                    {
                        // Branch Taken 
                        pInst->BranchOutcome = BRANCH_TAKEN;
                    }
                    else
                    {
                        // Branch Not Taken
                        pInst->BranchOutcome = BRANCH_NOT_TAKEN;
                    }

                    // Mark the instruction to be ready
                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                    break;

                case OPCODE_BNE:
                    if (pContext->RS[CurrRSEntry].Vj != pContext->RS[CurrRSEntry].Vk)
                    {
                        // Branch Taken 
                        pInst->BranchOutcome = BRANCH_TAKEN;
                    }
                    else
                    {
                        // Branch Not Taken
                        pInst->BranchOutcome = BRANCH_NOT_TAKEN;
                    }

                    // Mark the instruction to be ready
                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                    break;

                case OPCODE_BLEZ:
                    if (pContext->RS[CurrRSEntry].Vj <= 0)
                    {
                        // Branch Taken 
                        pInst->BranchOutcome = BRANCH_TAKEN;
                    }
                    else
                    {
                        // Branch Not Taken
                        pInst->BranchOutcome = BRANCH_NOT_TAKEN;
                    }

                    // Mark the instruction to be ready
                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                    break;

                case OPCODE_BGTZ:
                    if (pContext->RS[CurrRSEntry].Vj > 0)
                    {
                        // Branch Taken 
                        pInst->BranchOutcome = BRANCH_TAKEN;
                    }
                    else
                    {
                        // Branch Not Taken
                        pInst->BranchOutcome = BRANCH_NOT_TAKEN;
                    }

                    // Mark the instruction to be ready
                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                    break;

                case OPCODE_ADDI:
                case OPCODE_ADDIU:
                    pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj + pContext->RS[CurrRSEntry].Vk;
                    break;

                case OPCODE_SLTI:
                    pContext->RS[CurrRSEntry].Result = (pContext->RS[CurrRSEntry].Vj < pContext->RS[CurrRSEntry].Vk) ? 1 : 0;
                    break;

                case OPCODE_LW:
                        // For Address Generation, all Load/Stores ahead of this store should have their address generated! 
                        // Go through current ROB entries and make this check
                        for (count = 0; count < MAX_ROB_ENTRIES; count++)
                        {
                            if (pContext->ROB[count].Busy &&
                                pContext->ROB[count].pInst != pInst &&
                                pContext->ROB[count].pInst->IsLoadOrStoreInst &&
                                pContext->ROB[count].pInst->InstIssueNum < pInst->InstIssueNum &&
                                !pContext->ROB[count].pInst->IsLoadOrStoreAddrComputed)
                            {
                                ComputeAddress = false;
                                break;
                            }
                        }

                        if (ComputeAddress)
                        {
                            // Proceed with Address Computation for Load
                            pContext->RS[CurrRSEntry].Addr = pContext->RS[CurrRSEntry].Vj + pContext->RS[CurrRSEntry].Addr;
                            pInst->IsLoadOrStoreAddrComputed = true;
                        }
                    break;

                case OPCODE_SW:
                    // For Address Generation, all Load/Stores ahead of this store should have their address generated! 
                    // Go through current ROB entries and make this check
                    for (count = 0; count < MAX_ROB_ENTRIES; count++)
                    {
                        if (pContext->ROB[count].Busy &&
                            pContext->ROB[count].pInst->InstAddress != pInst->InstAddress &&
                            pContext->ROB[count].pInst->IsLoadOrStoreInst &&
                            pContext->ROB[count].pInst->InstIssueNum < pInst->InstIssueNum &&
                            !pContext->ROB[count].pInst->IsLoadOrStoreAddrComputed)
                        {
                            ComputeAddress = false;
                            break;
                        }
                    }

                    if (ComputeAddress)
                    {
                        // Proceed with Address Computation for Store 
                        pContext->RS[CurrRSEntry].Addr = pContext->RS[CurrRSEntry].Vj + pContext->RS[CurrRSEntry].Addr;
                        pInst->IsLoadOrStoreAddrComputed = true;
                    }

                    // Update the value in ROB
                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Value = pContext->RS[CurrRSEntry].Vk;

                    // Mark the instruction to be ready
                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                    // Write the result of the Address to the ROB 
                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Addr = pContext->RS[CurrRSEntry].Addr;

                    break;
                    
                default:
                    // should not reach here!
                    break;
                }

                // Update PC here itself 
                if (pInst->IsBranchInst || pInst->IsJumpInst)
                {
                    if (pInst->BranchOutcome == BRANCH_NOT_TAKEN && pInst->BranchPred == BRANCH_TAKEN)
                    {
                        // Update PC to PC + 4
                        pContext->CurrentPC = pInst->InstAddress + 4;

                        // Cancel all Instructions fetched after Branch in ROB
                        for (ROBCount = 0; ROBCount < MAX_ROB_ENTRIES; ROBCount++)
                        {
                            if (pContext->ROB[ROBCount].pInst->InstIssueNum > pInst->InstIssueNum)
                            {
                                // Free up ROB and RS
                                pContext->ROB[ROBCount].UpdateBusyStatus = true;

                                for (RSCount = 0; RSCount < MAX_RS_ENTRIES; RSCount++)
                                {
                                    if (pContext->RS[RSCount].Dest == ROBCount)
                                    {
                                        pContext->RS[RSCount].UpdateBusyStatus = true;
                                        break;
                                    }
                                }
                            }
                        }

                        // Also remove the last instruction from Instruction Queue
                        pContext->RemoveInstFromQueue = true;
                    }

                    if (pInst->BranchOutcome == BRANCH_TAKEN && (pInst->BranchPred == BRANCH_NOT_TAKEN || pInst->BranchPred == NOT_SET))
                    {
                        // Update PC to new Target
                        pContext->CurrentPC = checkInstructionInBTB(pContext, pInst->InstAddress)->TargetAddress;

                        // Cancel all Instructions fetched after Branch in ROB
                        for (ROBCount = 0; ROBCount < MAX_ROB_ENTRIES; ROBCount++)
                        {
                            if (pContext->ROB[ROBCount].pInst->InstIssueNum > pInst->InstIssueNum)
                            {
                                // Free up ROB and RS
                                pContext->ROB[ROBCount].UpdateBusyStatus = true;

                                for (RSCount = 0; RSCount < MAX_RS_ENTRIES; RSCount++)
                                {
                                    if (pContext->RS[RSCount].Dest == ROBCount)
                                    {
                                        pContext->RS[RSCount].UpdateBusyStatus = true;
                                        break;
                                    }
                                }
                            }
                        }

                        // Also remove the last instruction from Instruction Queue
                        pContext->RemoveInstFromQueue = true;
                    }
                }

                if (pInst->IsBranchInst || pInst->IsJumpInst)
                {
                    pInst->UpdatePred = true;
                }

                // Move to the next stage in next cycle 
                pContext->RS[CurrRSEntry].pInst->UpdatePipelineStage = true;
            }
        }
    }
}

void writebackInst(MipsSimContext *pContext)
{
    UINT    currRSEntry = 0;
    UINT    count = 0;
    bool    AccessMem = true;

    // Check if there is a instruction in RS ready to be write back 
    for (currRSEntry = 0; currRSEntry < MAX_RS_ENTRIES; currRSEntry++)
    {
        if (pContext->RS[currRSEntry].Busy && pContext->RS[currRSEntry].pInst->PipelineStage == INSTRUCTION_WRITE_MEM)
        {
            // If its a load instruction check if the Mem Access Is Done
            if (pContext->RS[currRSEntry].pInst->Opcode == OPCODE_LW && !pContext->RS[currRSEntry].pInst->IsLoadMemAccessDone)
            {
                // Do the memory access in this cycle
                // Load takes 2 cycles in write back stage 
                // First Check if there is no early store with the same address 
                for (count = 0; count < MAX_ROB_ENTRIES; count++)
                {
                    if (pContext->ROB[count].Busy &&
                        pContext->ROB[count].pInst != pContext->RS[currRSEntry].pInst &&
                        pContext->ROB[count].pInst->Opcode == OPCODE_SW &&
                        pContext->ROB[count].pInst->InstIssueNum < pContext->RS[currRSEntry].pInst->InstIssueNum &&
                        !pContext->ROB[count].Addr == pContext->RS[currRSEntry].Addr)
                    {
                        AccessMem = false;
                    }
                }

                if (AccessMem)
                {
                    pContext->RS[currRSEntry].Result = getDataFromList(pContext, pContext->RS[currRSEntry].Addr)->DataDWord;
                }
                
                pContext->RS[currRSEntry].pInst->IsLoadMemAccessDone = true;
                continue;
            }

            // Check if any RS is waiting for this result 
            for (count = 0; count < MAX_RS_ENTRIES; count++)
            {
                // Update Qj if any instruction is waiting for result
                if (pContext->RS[count].Qj == pContext->RS[currRSEntry].Dest)
                {
                    pContext->RS[count].Vj = pContext->RS[currRSEntry].Result;
                    pContext->RS[count].Qj = MAX_ROB_ENTRIES;
                }

                // Update Qk if any instruction is waiting for result
                if (pContext->RS[count].Qk == pContext->RS[currRSEntry].Dest)
                {
                    pContext->RS[count].Vk = pContext->RS[currRSEntry].Result;
                    pContext->RS[count].Qk = MAX_ROB_ENTRIES;
                }
            }

            // Update the ROB with the result and make it ready state
            pContext->ROB[pContext->RS[currRSEntry].Dest].Value = pContext->RS[currRSEntry].Result;
            pContext->ROB[pContext->RS[currRSEntry].Dest].Ready = true;
        
            // Move Inst to next stage 
            pContext->RS[currRSEntry].pInst->UpdatePipelineStage = true;
        }
    }
}

void commitInst(MipsSimContext *pContext)
{
    UINT    ROBEntry = 0;
    Data*   pData = NULL;
    UINT    ROBCount = 0, RSCount = 0;
    Instruction *pInst = NULL;

    // Check that ROB is not empty! 
    if (!IsROBEmpty(pContext))
    {
        // Get the oldest instruction in the ROB to commit
        ROBEntry = getROBEntryOldestInst(pContext);

        // Check if the Inst is in the commit stage  
        if (pContext->ROB[ROBEntry].pInst->PipelineStage == INSTRUCTION_COMMIT)
        {
            if (pContext->ROB[ROBEntry].pInst->Opcode == OPCODE_SW)
            {
                // Update the mem location with the value
                pData = getDataFromList(pContext, pContext->ROB[ROBEntry].Addr);
                pData->DataDWord = pContext->ROB[ROBEntry].Value;
                setDataToList(pContext, pData);
            }
            else
            {
                if (!pContext->ROB[ROBEntry].pInst->IsBranchInst && !pContext->ROB[ROBEntry].pInst->IsJumpInst && !pContext->ROB[ROBEntry].pInst->IsBreakOrNopInst)
                {
                    // Update the destination register 
                    RegLookUpTable[pContext->ROB[ROBEntry].Dest].RegValue = pContext->ROB[ROBEntry].Value;
                }
            }

            // Free up the ROB Entry & corresponding RS Entry
            pContext->ROB[ROBEntry].UpdateBusyStatus = true;

            for (RSCount = 0; RSCount < MAX_RS_ENTRIES; RSCount++)
            {
                if (pContext->RS[RSCount].Dest == ROBEntry)
                {
                    pContext->RS[RSCount].UpdateBusyStatus = true;
                    break;
                }
            }

            // Free up the register
            if (pContext->RegisterStat[pContext->ROB[ROBEntry].Dest].Reorder == ROBEntry)
            {
                pContext->RegisterStat[pContext->ROB[ROBEntry].Dest].Busy = false;
            }
        }
    }
}

void simulateExTomasulo(MipsSimContext *pContext)
{
    UINT count = 0;
    
    // Keep Executing till IQ is empty and all the ROB entries are free ! 
    do
    {
        // Increment Cycle No 
        pContext->CurrentCycle++;

//        if (pContext->CurrentCycle == 45) break;

        // I. Instruction Fetch
        fetchInst(pContext);

        // II. Instruction Issue/Decode 
        issueInst(pContext);

        // III. Instruction Execute
        executeInst(pContext);

        // IV. Instruction Write Result
        writebackInst(pContext);

        if (!pContext->Args.PrintFinalState)
        {
            // Print the Cycle output in the output txt file
            printOutputFile(pContext);
        }

        // V. Instruction Commit
        commitInst(pContext);

        // End of Cycle: Update the Instruction Pipeline Stage in RS
        for (count = 0; count < MAX_ROB_ENTRIES; count++)
        {
            if (pContext->ROB[count].Busy && pContext->ROB[count].pInst->UpdatePipelineStage)
            {
                // if there is a NOP or BREAK Instruction in ISSUE stage, update it directly to commit stage 
                if (pContext->ROB[count].pInst->IsBreakOrNopInst && pContext->ROB[count].pInst->PipelineStage == INSTRUCTION_ISSUE)
                {
                    pContext->ROB[count].pInst->PipelineStage = INSTRUCTION_COMMIT;
                }
                // SW, Jump or Branch dont have a write back stage, directly move them to Commit 
                else if ((pContext->ROB[count].pInst->IsBranchInst || pContext->ROB[count].pInst->IsJumpInst || pContext->ROB[count].pInst->Opcode == OPCODE_SW) &&
                    pContext->ROB[count].pInst->PipelineStage == INSTRUCTION_EX)
                {
                    pContext->ROB[count].pInst->PipelineStage = INSTRUCTION_COMMIT;
                }
                else
                {
                    // Move to the next Pipeline Stage
                    pContext->ROB[count].pInst->PipelineStage++;
                }
                pContext->ROB[count].pInst->UpdatePipelineStage = false;
            }
        }
        
        // Update the Busy status for RS entries 
        for (count = 0; count < MAX_RS_ENTRIES; count++)
        {
            if (pContext->RS[count].Busy)
            {
                if (pContext->RS[count].pInst->UpdatePred)
                {
                    updatePredAtEXStage(pContext, pContext->RS[count].pInst->InstAddress, pContext->RS[count].pInst->BranchOutcome);
                    pContext->RS[count].pInst->UpdatePred = false;
                }
            }

            if (pContext->RS[count].UpdateBusyStatus)
            {
                pContext->RS[count].Busy = false;
                pContext->RS[count].UpdateBusyStatus = false;
            }
        }

        // Update the Busy status for ROB entries 
        for (count = 0; count < MAX_ROB_ENTRIES; count++)
        {
            if (pContext->ROB[count].UpdateBusyStatus)
            {
                pContext->ROB[count].Busy = false;
                pContext->ROB[count].UpdateBusyStatus = false;
            }
        }

        // Remove Inst from Queue if required 
        if (pContext->RemoveInstFromQueue)
        {
            pContext->InstQueue = NULL;
            pContext->RemoveInstFromQueue = false;
        }

    } while (!IsSimComplete(pContext));

    // Print one more cycle 
    pContext->CurrentCycle++;

    // Print the Cycle output in the output txt file
    printOutputFile(pContext);

}
