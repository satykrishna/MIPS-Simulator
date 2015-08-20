//
// This file contains the structure definitions to simulate
// MIPS instruction execution using Tomasulo Algorithm 
// with Reoder Buffer  
//

#ifndef SIMTOMASULO_H_
#define SIMTOMASULO_H_

#include "InstParser.h"

#define MAX_ROB_ENTRIES 6
#define MAX_RS_ENTRIES  10

// This structure defines an entry for Reservation Station
typedef struct _RSEntry
{
    bool            UpdateBusyStatus;
    bool            Busy;
    Instruction     *pInst;
    INT             Vj;
    INT             Vk;
    UINT            Qj;
    UINT            Qk;
    UINT            Dest;
    INT             Addr;
    INT             Result;
}RSEntry;

// This structure defines an entry for ReOrder Buffer 
typedef struct _ROBEntry
{
    bool                UpdateBusyStatus;
    bool                Busy;
    Instruction         *pInst;
    bool                Ready;
    UINT                Dest;
    INT                 Value;
    INT                 Addr;
}ROBEntry;

// This struture defines an entry for Register Status 
typedef struct _RegisterStatEntry
{
    bool    Busy;
    UINT    Reorder;
}RegisterStatEntry;

#endif

