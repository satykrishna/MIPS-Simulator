//
// This file contains functions to decode instruction 
// for all opcodes 
//

#include "InstParser.h"
#include "MIPSSim.h"

// INSTRUCTION FORMATS

// NOP
// BREAK
// J target
// SLL      rd, rt, sa
// SRL      rd, rt, sa
// SRA      rd, rt, sa
// ADD      rd, rs, rt
// ADDU     rd, rs, rt
// SUB      rd, rs, rt
// SUBU     rd, rs, rt
// AND      rd, rs, rt
// OR       rd, rs, rt
// XOR      rd, rs, rt
// NOR      rd, rs, rt
// SLT      rd, rs, rt
// SLTU     rd, rs, rt
// BLTZ     rs, Immediate
// BGEZ     rs, Immediate
// BEQ      rs, rt, Immediate
// BNE      rs, rt, Immediate
// BLEZ     rs, Immediate
// BGTZ     rs, Immediate
// ADDI     rt, rs, Immediate
// ADDIU    rt, rs, Immediate
// SLTI     rt, rs, Immediate
// SW       rt, Immediate(rs)
// LW       rt, Immediate(rs)

void decodeSLL(Instruction *pInst)
{
    // Update Fields
    pInst->InstructionType = R_TYPE_INSTRUCTION;

    // if rd == R0, rt == R0 & sa == 0
    // This will be decoded as NOP instruction instead
    if (pInst->InstructionDWord.RType.Rt == 0 && pInst->InstructionDWord.RType.Rd == 0 && pInst->InstructionDWord.RType.Shamt == 0)
    {
        pInst->InstructionName = "NOP";

        pInst->IsBreakOrNopInst = true;

        //Format : NOP 
        // Build the Inst String
        memset(pInst->InstString, 0, sizeof(pInst->InstString));
        strcpy(pInst->InstString, pInst->InstructionName);
        pInst->InstStringLength = strlen(pInst->InstString);
    }
    else
    {
        pInst->InstructionName = "SLL";
        pInst->Rt = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegName;
        pInst->Rd = RegLookUpTable[pInst->InstructionDWord.RType.Rd].RegName;
        sprintf(pInst->Shamt, "%d", pInst->InstructionDWord.RType.Shamt);

        pInst->IsRtUsedAsOperand = true;
        pInst->IsRdUsedAsDest = true;
        pInst->IsShiftALUInst = true;

        // Format : SLL rd, rt, sa 
        // Build the Inst String
        memset(pInst->InstString, 0, sizeof(pInst->InstString));
        strcpy(pInst->InstString, pInst->InstructionName);
        strcat(pInst->InstString, " ");
        strcat(pInst->InstString, pInst->Rd);
        strcat(pInst->InstString, ", ");
        strcat(pInst->InstString, pInst->Rt);
        strcat(pInst->InstString, ", #");
        strcat(pInst->InstString, pInst->Shamt);
        pInst->InstStringLength = strlen(pInst->InstString);
    }
}

void decodeSRL(Instruction *pInst)
{
    // Update Fields
    pInst->InstructionType = R_TYPE_INSTRUCTION;
    pInst->InstructionName = "SRL";
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegName;
    pInst->Rd = RegLookUpTable[pInst->InstructionDWord.RType.Rd].RegName;
    sprintf(pInst->Shamt, "%d", pInst->InstructionDWord.RType.Shamt);

    pInst->IsRtUsedAsOperand = true;
    pInst->IsRdUsedAsDest = true;
    pInst->IsShiftALUInst = true;

    // Format : SRL rd, rt, sa 
    // Build the Inst String
    memset(pInst->InstString, 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rd);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rt);
    strcat(pInst->InstString, ", #");
    strcat(pInst->InstString, pInst->Shamt);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeSRA(Instruction *pInst)
{
    // Update Fields
    pInst->InstructionType = R_TYPE_INSTRUCTION;
    pInst->InstructionName = "SRA";
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegName;
    pInst->Rd = RegLookUpTable[pInst->InstructionDWord.RType.Rd].RegName;
    sprintf(pInst->Shamt, "%d", pInst->InstructionDWord.RType.Shamt);

    pInst->IsRtUsedAsOperand = true;
    pInst->IsRdUsedAsDest = true;
    pInst->IsShiftALUInst = true;

    // Format : SRA rd, rt, sa 
    // Build the Inst String
    memset(pInst->InstString, 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rd);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rt);
    strcat(pInst->InstString, ", #");
    strcat(pInst->InstString, pInst->Shamt);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeBREAK(Instruction *pInst)
{
    // Update Fields
    pInst->InstructionType = R_TYPE_INSTRUCTION;
    pInst->InstructionName = "BREAK";

    pInst->IsBreakOrNopInst = true;

    // Format : BREAK
    // Build the Inst String 
    memset(&pInst->InstString[0], 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeADD(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = R_TYPE_INSTRUCTION;
    pInst->InstructionName = "ADD";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.RType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegName;
    pInst->Rd = RegLookUpTable[pInst->InstructionDWord.RType.Rd].RegName;

    pInst->IsRtUsedAsOperand = true;
    pInst->IsRsUsedAsOperand = true;
    pInst->IsRdUsedAsDest = true;

    // Format : ADD rd, rs, rt
    // Build the Inst String
    memset(pInst->InstString, 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rd);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rt);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeADDU(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = R_TYPE_INSTRUCTION;
    pInst->InstructionName = "ADDU";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.RType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegName;
    pInst->Rd = RegLookUpTable[pInst->InstructionDWord.RType.Rd].RegName;

    pInst->IsRtUsedAsOperand = true;
    pInst->IsRsUsedAsOperand = true;
    pInst->IsRdUsedAsDest = true;

    // Format : ADDU rd, rs, rt
    // Build the Inst String
    memset(pInst->InstString, 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rd);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rt);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeSUB(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = R_TYPE_INSTRUCTION;
    pInst->InstructionName = "SUB";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.RType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegName;
    pInst->Rd = RegLookUpTable[pInst->InstructionDWord.RType.Rd].RegName;

    pInst->IsRtUsedAsOperand = true;
    pInst->IsRsUsedAsOperand = true;
    pInst->IsRdUsedAsDest = true;

    // Format : SUB rd, rs, rt
    // Build the Inst String
    memset(pInst->InstString, 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rd);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rt);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeSUBU(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = R_TYPE_INSTRUCTION;
    pInst->InstructionName = "SUBU";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.RType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegName;
    pInst->Rd = RegLookUpTable[pInst->InstructionDWord.RType.Rd].RegName;

    pInst->IsRtUsedAsOperand = true;
    pInst->IsRsUsedAsOperand = true;
    pInst->IsRdUsedAsDest = true;

    // Format : SUBU rd, rs, rt
    // Build the Inst String
    memset(pInst->InstString, 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rd);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rt);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeAND(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = R_TYPE_INSTRUCTION;
    pInst->InstructionName = "AND";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.RType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegName;
    pInst->Rd = RegLookUpTable[pInst->InstructionDWord.RType.Rd].RegName;

    pInst->IsRtUsedAsOperand = true;
    pInst->IsRsUsedAsOperand = true;
    pInst->IsRdUsedAsDest = true;

    // Format : AND rd, rs, rt
    // Build the Inst String
    memset(pInst->InstString, 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rd);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rt);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeOR(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = R_TYPE_INSTRUCTION;
    pInst->InstructionName = "OR";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.RType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegName;
    pInst->Rd = RegLookUpTable[pInst->InstructionDWord.RType.Rd].RegName;

    pInst->IsRtUsedAsOperand = true;
    pInst->IsRsUsedAsOperand = true;
    pInst->IsRdUsedAsDest = true;

    // Format : OR rd, rs, rt
    // Build the Inst String
    memset(pInst->InstString, 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rd);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rt);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeXOR(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = R_TYPE_INSTRUCTION;
    pInst->InstructionName = "XOR";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.RType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegName;
    pInst->Rd = RegLookUpTable[pInst->InstructionDWord.RType.Rd].RegName;

    pInst->IsRtUsedAsOperand = true;
    pInst->IsRsUsedAsOperand = true;
    pInst->IsRdUsedAsDest = true;

    // Format : XOR rd, rs, rt
    // Build the Inst String
    memset(pInst->InstString, 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rd);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rt);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeNOR(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = R_TYPE_INSTRUCTION;
    pInst->InstructionName = "NOR";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.RType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegName;
    pInst->Rd = RegLookUpTable[pInst->InstructionDWord.RType.Rd].RegName;

    pInst->IsRtUsedAsOperand = true;
    pInst->IsRsUsedAsOperand = true;
    pInst->IsRdUsedAsDest = true;

    // Format : NOR rd, rs, rt
    // Build the Inst String
    memset(pInst->InstString, 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rd);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rt);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeSLT(Instruction *pInst)
{
    // Update Fields
    pInst->InstructionType = R_TYPE_INSTRUCTION;
    pInst->InstructionName = "SLT";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.RType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegName;
    pInst->Rd = RegLookUpTable[pInst->InstructionDWord.RType.Rd].RegName;

    pInst->IsRtUsedAsOperand = true;
    pInst->IsRsUsedAsOperand = true;
    pInst->IsRdUsedAsDest = true;

    // Format : SLT rd, rs, rt 
    // Build the Inst String
    memset(pInst->InstString, 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rd);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rt);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeSLTU(Instruction *pInst)
{
    // Update Fields
    pInst->InstructionType = R_TYPE_INSTRUCTION;
    pInst->InstructionName = "SLTU";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.RType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegName;
    pInst->Rd = RegLookUpTable[pInst->InstructionDWord.RType.Rd].RegName;

    pInst->IsRtUsedAsOperand = true;
    pInst->IsRsUsedAsOperand = true;
    pInst->IsRdUsedAsDest = true;

    // Format : SLTU rd, rs, rt 
    // Build the Inst String
    memset(pInst->InstString, 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rd);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rt);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeBLTZ(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = I_TYPE_INSTRUCTION;
    pInst->InstructionName = "BLTZ";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.IType.Rs].RegName;
    sprintf(pInst->Immediate, "%d", pInst->InstructionDWord.IType.SignedImmediate * 4);

    pInst->IsBranchInst = true;
    pInst->IsRsUsedAsOperand = true;

    // Format : BLTZ rs, Immediate
    // Build the Inst String
    memset(&pInst->InstString[0], 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", #");
    strcat(pInst->InstString, pInst->Immediate);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeBGEZ(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = I_TYPE_INSTRUCTION;
    pInst->IsBranchInst = true;
    pInst->InstructionName = "BGEZ";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.IType.Rs].RegName;
    sprintf(pInst->Immediate, "%d", pInst->InstructionDWord.IType.SignedImmediate * 4);

    pInst->IsBranchInst = true;
    pInst->IsRsUsedAsOperand = true;

    // Format : BGEZ rs, Immediate
    // Build the Inst String
    memset(&pInst->InstString[0], 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", #");
    strcat(pInst->InstString, pInst->Immediate);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeJ(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = J_TYPE_INSTRUCTION;
    pInst->InstructionName = "J";
    sprintf(pInst->JumpTarget, "%u", pInst->InstructionDWord.JType.Target * 4);

    pInst->IsJumpInst = true;

    // Format : J target
    // Build the Inst String
    memset(&pInst->InstString[0], 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, "#");
    strcat(pInst->InstString, pInst->JumpTarget);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeBEQ(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = I_TYPE_INSTRUCTION;
    pInst->InstructionName = "BEQ";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.IType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.IType.Rt].RegName;
    sprintf(pInst->Immediate, "%d", pInst->InstructionDWord.IType.SignedImmediate * 4);

    pInst->IsBranchInst = true;
    pInst->IsRsUsedAsOperand = true;
    pInst->IsRtUsedAsOperand = true;

    // Format : BEQ rs, rt, Immediate
    // Build the Inst String
    memset(&pInst->InstString[0], 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rt);
    strcat(pInst->InstString, ", #");
    strcat(pInst->InstString, pInst->Immediate);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeBNE(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = I_TYPE_INSTRUCTION;
    pInst->InstructionName = "BNE";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.IType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.IType.Rt].RegName;
    sprintf(pInst->Immediate, "%d", pInst->InstructionDWord.IType.SignedImmediate * 4);

    pInst->IsBranchInst = true;
    pInst->IsRsUsedAsOperand = true;
    pInst->IsRtUsedAsOperand = true;

    // Format : BNE rs, rt, Immediate
    // Build the Inst String
    memset(&pInst->InstString[0], 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rt);
    strcat(pInst->InstString, ", #");
    strcat(pInst->InstString, pInst->Immediate);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeBLEZ(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = I_TYPE_INSTRUCTION;
    pInst->InstructionName = "BLEZ";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.IType.Rs].RegName;
    sprintf(pInst->Immediate, "%d", pInst->InstructionDWord.IType.SignedImmediate * 4);

    pInst->IsBranchInst = true;
    pInst->IsRsUsedAsOperand = true;

    // Format : BLEZ rs, Immediate
    // Build the Inst String
    memset(&pInst->InstString[0], 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", #");
    strcat(pInst->InstString, pInst->Immediate);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeBGTZ(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = I_TYPE_INSTRUCTION;
    pInst->InstructionName = "BGTZ";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.IType.Rs].RegName;
    sprintf(pInst->Immediate, "%d", pInst->InstructionDWord.IType.SignedImmediate * 4);

    pInst->IsBranchInst = true;
    pInst->IsRsUsedAsOperand = true;

    // Format : BGTZ rs, Immediate
    // Build the Inst String
    memset(&pInst->InstString[0], 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", #");
    strcat(pInst->InstString, pInst->Immediate);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeADDI(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = I_TYPE_INSTRUCTION;
    pInst->InstructionName = "ADDI";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.IType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.IType.Rt].RegName;
    sprintf(pInst->Immediate, "%d", pInst->InstructionDWord.IType.SignedImmediate);

    pInst->IsRsUsedAsOperand = true;
    pInst->IsRtUsedAsDest = true;
    pInst->IsImmALUInst = true;

    // Format : ADDI rt, rs, Immediate
    // Build the Inst String
    memset(&pInst->InstString[0], 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rt);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", #");
    strcat(pInst->InstString, pInst->Immediate);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeADDIU(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = I_TYPE_INSTRUCTION;
    pInst->InstructionName = "ADDIU";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.IType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.IType.Rt].RegName;
    sprintf(pInst->Immediate, "%d", pInst->InstructionDWord.IType.SignedImmediate);

    pInst->IsRsUsedAsOperand = true;
    pInst->IsRtUsedAsDest = true;
    pInst->IsImmALUInst = true;

    // Format : ADDIU rt, rs, Immediate
    // Build the Inst String
    memset(&pInst->InstString[0], 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rt);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", #");
    strcat(pInst->InstString, pInst->Immediate);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeSLTI(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = I_TYPE_INSTRUCTION;
    pInst->InstructionName = "SLTI";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.IType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.IType.Rt].RegName;
    sprintf(pInst->Immediate, "%d", pInst->InstructionDWord.IType.SignedImmediate);

    pInst->IsRsUsedAsOperand = true;
    pInst->IsRtUsedAsDest = true;
    pInst->IsImmALUInst = true;

    // Format : SLTI rt, rs, Immediate
    // Build the Inst String
    memset(&pInst->InstString[0], 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rt);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ", #");
    strcat(pInst->InstString, pInst->Immediate);
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeSW(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = I_TYPE_INSTRUCTION;
    pInst->InstructionName = "SW";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.IType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.IType.Rt].RegName;
    sprintf(pInst->Immediate, "%d", pInst->InstructionDWord.IType.SignedImmediate);

    pInst->IsLoadOrStoreInst = true;
    pInst->IsRsUsedAsOperand = true;
    pInst->IsRtUsedAsOperand = true;

    // Format : SW rt, Immediate(rs)
    // Build the Inst String
    memset(&pInst->InstString[0], 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rt);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Immediate);
    strcat(pInst->InstString, "(");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ")");
    pInst->InstStringLength = strlen(pInst->InstString);
}

void decodeLW(Instruction *pInst)
{
    //Update Fields
    pInst->InstructionType = I_TYPE_INSTRUCTION;
    pInst->InstructionName = "LW";
    pInst->Rs = RegLookUpTable[pInst->InstructionDWord.IType.Rs].RegName;
    pInst->Rt = RegLookUpTable[pInst->InstructionDWord.IType.Rt].RegName;
    sprintf(pInst->Immediate, "%d", pInst->InstructionDWord.IType.SignedImmediate);

    pInst->IsLoadOrStoreInst = true;
    pInst->IsRsUsedAsOperand = true;
    pInst->IsRtUsedAsDest = true;

    // Format : LW rt, Immediate(rs)
    // Build the Inst String
    memset(&pInst->InstString[0], 0, sizeof(pInst->InstString));
    strcpy(pInst->InstString, pInst->InstructionName);
    strcat(pInst->InstString, " ");
    strcat(pInst->InstString, pInst->Rt);
    strcat(pInst->InstString, ", ");
    strcat(pInst->InstString, pInst->Immediate);
    strcat(pInst->InstString, "(");
    strcat(pInst->InstString, pInst->Rs);
    strcat(pInst->InstString, ")");
    pInst->InstStringLength = strlen(pInst->InstString);
}








