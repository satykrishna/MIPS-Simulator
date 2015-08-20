//
// This header file contains all the function definitions 
//

#ifndef FUNCPROTO_H_
#define FUNCPROTO_H_

#include "InstParser.h"
#include "MIPSSim.h"
#include "SimTomasulo.h"
#include "btb.h"

bool    verifyInOutFiles(MipsSimContext *pContext);
void    closeFiles(MipsSimContext *pContext);
INT     readSignedDWordFile(FILE *fp);
UINT    readUnsignedDWordFile(FILE *fp);
void    printOutputFile(MipsSimContext *pContext);

void printArguementUsage();
bool parseArguements(MipsSimContext *pContext, char *argv[]);
void swapBytes(char *Byte1, char* Byte2);
void parseOpcode(MipsSimContext *pContext, Instruction *pInst);
void addAllInstToContext(MipsSimContext *pContext);
void addAllDataToContext(MipsSimContext *pContext);
void parseInputFile(MipsSimContext *pContext);

void            addInstToList(MipsSimContext *pContext, Instruction *pInst);
void            addDataToList(MipsSimContext *pContext, INT ReadDWord);
Instruction*    getInstFromList(MipsSimContext *pContext, UINT InstAddress);
Data*           getDataFromList(MipsSimContext *pContext, UINT Addr);
void            setDataToList(MipsSimContext *pContext, Data *pData);
void            addInstToInstQueue(MipsSimContext *pContext, Instruction *pInst);
void            moveInstToISStage(MipsSimContext *pContext);
bool            IsInstQueueEmpty(MipsSimContext *pContext);
Instruction*    getInstToIssue(MipsSimContext *pContext);
void            removeInstFromInstQueue(MipsSimContext *pContext, Instruction *pInst);
UINT            reorderROBInst(MipsSimContext *pContext, Instruction *ROBInst);
UINT            reorderRSInst(MipsSimContext *pContext, Instruction *RSInst);
UINT            getROBEntryOldestInst(MipsSimContext *pContext);
bool            IsROBEmpty(MipsSimContext *pContext);
    
void fetchInst(MipsSimContext *pContext);
void issueInst(MipsSimContext *pContext);
bool IsSimComplete(MipsSimContext *pContext);
void simulateExTomasulo(MipsSimContext *pContext);
bool getAvailableROBEntry(MipsSimContext* pContext, UINT* ROBEntry);
bool getAvailableRSEntry(MipsSimContext* pContext, UINT* RSEntry);

void decodeSLL(Instruction *pInst);
void decodeSRL(Instruction *pInst);
void decodeSRA(Instruction *pInst);
void decodeBREAK(Instruction *pInst);
void decodeADD(Instruction *pInst);
void decodeADDU(Instruction *pInst);
void decodeSUB(Instruction *pInst);
void decodeSUBU(Instruction *pInst);
void decodeAND(Instruction *pInst);
void decodeOR(Instruction *pInst);
void decodeXOR(Instruction *pInst);
void decodeNOR(Instruction *pInst);
void decodeSLT(Instruction *pInst);
void decodeSLTU(Instruction *pInst);
void decodeBLTZ(Instruction *pInst);
void decodeBGEZ(Instruction *pInst);
void decodeJ(Instruction *pInst);
void decodeBEQ(Instruction *pInst);
void decodeBNE(Instruction *pInst);
void decodeBLEZ(Instruction *pInst);
void decodeBGTZ(Instruction *pInst);
void decodeADDI(Instruction *pInst);
void decodeADDIU(Instruction *pInst);
void decodeSLTI(Instruction *pInst);
void decodeSW(Instruction *pInst);
void decodeLW(Instruction *pInst);

BTBEntry*   checkInstructionInBTB(MipsSimContext *pContext, UINT InstAddress);
UINT        getSizeofBTB(MipsSimContext *pContext);
BTBEntry*   findLRUEntry(MipsSimContext *pContext);
void        insertNewPredAtISStage(MipsSimContext *pContext, BTBEntry *newEntry);
UINT        getNextPCAddress(MipsSimContext *pContext, UINT InstAddress);
void        updatePredAtEXStage(MipsSimContext *pContext, UINT InstAddress, UINT PredState);

#endif
