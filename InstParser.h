//
// This file contains the structure definitions to parse 
// MIPS instructions 
//


#ifndef INSTPARSER_H_
#define INSTPARSER_H_
 
#include <stdbool.h>

typedef unsigned int UINT;
typedef signed int INT;

#define OpcodeMask          0xFC000000
#define OpcodeShift         26
#define MaxInstStringLength 255

// Type of instrcuction 
typedef enum _InstructionType
{
    R_TYPE_INSTRUCTION,
    I_TYPE_INSTRUCTION,
    J_TYPE_INSTRUCTION,
}InstructionType;

// Pipeline Stage of Instruction 
typedef enum _PipelineStage
{
    INSTRUCTION_NEW,
    INSTRUCTION_FETCH,
    INSTRUCTION_ISSUE,
    INSTRUCTION_EX,
    INSTRUCTION_WRITE_MEM,
    INSTRUCTION_COMMIT,
}PipelineStage;

 
// Definitions for Opcodes for all the Instructions 
#define OPCODE_SPECIAL  0x00
#define OPCODE_REGIMM   0x01
#define OPCODE_J        0x02
#define OPCODE_BEQ      0x04
#define OPCODE_BNE      0x05
#define OPCODE_BLEZ     0x06
#define OPCODE_BGTZ     0x07
#define OPCODE_ADDI     0x08
#define OPCODE_ADDIU    0x09
#define OPCODE_SLTI     0x0a
#define OPCODE_LW       0x23
#define OPCODE_SW       0x2b

// For Opcode SPECIAL, definitions for FUNC field
#define FUNC_SLL    0x00
#define FUNC_SRL    0x02
#define FUNC_SRA    0x03
#define FUNC_BREAK  0x0d
#define FUNC_ADD    0x20
#define FUNC_ADDU   0x21
#define FUNC_SUB    0x22
#define FUNC_SUBU   0x23
#define FUNC_AND    0x24
#define FUNC_OR     0x25
#define FUNC_XOR    0x26
#define FUNC_NOR    0x27
#define FUNC_SLT    0x2a
#define FUNC_SLTU   0x2b

// For Opcode REGIMM, definitions for RT field 
#define RT_BGEZ      0x1
#define RT_BLTZ     0x0

typedef struct _Reg
{
    UINT    RegNo;
    char*   RegName;
    INT     RegValue;
}Reg;

typedef Reg RegList[];
extern RegList RegLookUpTable;

// Structure Definition for
// R Type Instructions  :   BREAK, SLT, SLTU, SLL, SRL, SRA, SUB, SUBU, ADD, ADDU
//                          AND, OR, XOR, NOR, NOP
// I Type Instructions  :   LW, SW, BEQ, BNE, BGEZ, BGTZ, BLEZ, BLTZ, ADDI, ADDIU, SLTI
// J Type Instructions  :   J 
typedef struct _InstDWord
{
    union
    {
        UINT    InstDwordValue;
        struct
        {
            INT     SignedImmediate : 16;
            UINT    Rt          : 5;
            UINT    Rs          : 5;
            UINT    Opcode      : 6;
        }IType;
        struct
        {
            UINT    Function    : 6;
            UINT    Shamt       : 5;
            UINT    Rd          : 5;
            UINT    Rt          : 5;
            UINT    Rs          : 5;
            UINT    Opcode      : 6;
        }RType;
        struct
        {
            UINT    Target : 26;
            UINT    Opcode : 6;
        }JType;
    };
}InstDWord;

// For every instruction, build the basic structure
// Oppcode # 
// Opcode name  
typedef struct _Instruction
{
    InstructionType     InstructionType;
    InstDWord           InstructionDWord;
    UINT                Opcode;
    char*               InstructionName;
    char*               Rs;
    char*               Rt;
    char*               Rd;
    char                Shamt[20];
    char                Immediate[20];
    char                JumpTarget[20];
    char                InstString[MaxInstStringLength];
    UINT                InstStringLength;
    UINT                InstAddress;
    PipelineStage       PipelineStage;
    bool                UpdatePipelineStage;
    bool                IsBranchInst;
    bool                IsJumpInst;
    bool                IsLoadOrStoreInst;
    bool                IsRsUsedAsOperand;
    bool                IsRtUsedAsOperand;
    bool                IsRtUsedAsDest;
    bool                IsRdUsedAsDest;
    bool                IsImmALUInst;
    bool                IsShiftALUInst;
    bool                IsBreakOrNopInst;
    bool                IsLoadOrStoreAddrComputed;
    bool                IsLoadMemAccessDone;
    UINT                BranchPred;
    UINT                BranchOutcome;
    UINT                InstIssueNum;
    bool                UpdatePred;
    struct _Instruction *next;
}Instruction;

#endif
