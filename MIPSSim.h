//
// This is the main heder file for MIPS Disassembler 
//

#ifndef MIPSSIM_H_
#define MIPSSIM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "SimTomasulo.h"
#include "InstParser.h"
#include "btb.h"

#define BUFFER_SIZE         1532
#define INST_START_ADDRESS  600
#define DATA_START_ADDRESS  716

// sturture to hold a DWORD 
union _DWord
{
    union
    {
        UINT    UnsignedDwordValue;
        INT     SignedDWordValue;
    };
    char    Byte[4];
};

typedef struct _MipsSimArgs
{
    char*   InputFilename;  // Input file name 
    char*   OutputFilename; // Output file name 
    int     StartCycleNum;
    int     EndCycleNum;
    bool    PrintFinalState;
    bool    PrintAllCycles;
}MipsSimArgs;

typedef struct _FileHandle
{
    FILE   *InputFileHandle;
    FILE   *OutputFileHandle;
}FileHandle;

typedef struct _Data
{
    UINT            DataAddress;
    INT             DataDWord;
    struct _Data    *next;
}Data;

typedef struct _MipsSimContext
{
    MipsSimArgs         Args;
    FileHandle          FileHandle;
    UINT                InputFileLength;
    UINT                NoOfDWordsRead;
    bool                IsBreakInst;
    bool                IsEndOfFileHit;
    char                Buffer[BUFFER_SIZE];
    UINT                currentBufferPos;
    UINT                CurrentAddress;
    UINT                InstructionCount;
    Instruction*        InstructionList;
    UINT                DataCount;
    Data*               DataList;
    RSEntry             RS[MAX_RS_ENTRIES];
    ROBEntry            ROB[MAX_ROB_ENTRIES];
    BTBEntry            *BTB;
    RegisterStatEntry   RegisterStat[32];
    Instruction*        InstQueue;
    UINT                CurrentPC;
    UINT                CurrentCycle;
    UINT                CurrentInstIssueNum;
    bool                RemoveInstFromQueue;
}MipsSimContext;

#endif

