//
// This file contains functions to add/search elements 
// in the list in context 
//

#include "FuncProto.h"

// addInstToList()
// This functions adds an instruction to the end of 
// instruction linked list in the context
void addInstToList(MipsSimContext *pContext, Instruction  *pInst)
{
    Instruction     *pLocalInst = NULL;
    Instruction     *pCurrInst = NULL;

    // Allocate space for a new Instruction
    pLocalInst = (Instruction*)malloc(sizeof(Instruction));
    memcpy(pLocalInst, pInst, sizeof(Instruction));

    // Add it to the List in the Context
    if (pContext->InstructionList == NULL)
    {
        // List is empty! 
        pContext->InstructionList = pLocalInst;
    }
    else
    {
        // Traverse to the end of the List and add it there
        pCurrInst = pContext->InstructionList;
        while (pCurrInst->next != NULL)
        {
            pCurrInst = pCurrInst->next;
        }
        pCurrInst->next = pLocalInst;
    }
}

// addDataToList()
// This functions adds a data to the end of 
// data linked list in the context
void addDataToList(MipsSimContext *pContext, INT ReadDWord)
{
    Data    *pLocalData = NULL;
    Data    *pCurrData = NULL;

    // Allocate space for a new Instruction
    pLocalData = (Data*)malloc(sizeof(Data));
    pLocalData->DataAddress = pContext->CurrentAddress;
    pLocalData->DataDWord = ReadDWord;
    pLocalData->next = NULL;

    // Add it to the List in the Context
    if (pContext->DataList == NULL)
    {
        // List is empty! 
        pContext->DataList = pLocalData;
    }
    else
    {
        // Traverse to the end of the List and add it there
        pCurrData = pContext->DataList;
        while (pCurrData->next != NULL)
        {
            pCurrData = pCurrData->next;
        }
        pCurrData->next = pLocalData;
    }
}

// getInstFromList()
// This function retrieves the Inst from the List at the current PC
Instruction* getInstFromList(MipsSimContext *pContext, UINT InstAddress)
{
    Instruction     *pCurrInst = NULL;
    Instruction     *pTempInst = NULL;

    if (pContext->InstructionList != NULL)
    {
        pCurrInst = pContext->InstructionList;

        if (pCurrInst->InstAddress == InstAddress)
        {
            pTempInst = pCurrInst;
        }
        else
        {
            while (pCurrInst->next != NULL)
            {
                pCurrInst = pCurrInst->next;
                if (pCurrInst->InstAddress == InstAddress)
                {
                    pTempInst = pCurrInst;
                    break;
                }
            }
        }
    }

    return pTempInst;
}

// getDataFromList()
// This function retrieves the Data from the List at the address 
Data* getDataFromList(MipsSimContext *pContext, UINT Addr)
{
    Data     *pCurrData = NULL;
    Data     *pTempData = NULL;

    if (pContext->DataList != NULL)
    {
        pCurrData = pContext->DataList;

        if (pCurrData->DataAddress == Addr)
        {
            pTempData = pCurrData;
        }
        else
        {
            while (pCurrData->next != NULL)
            {
                pCurrData = pCurrData->next;
                if (pCurrData->DataAddress == Addr)
                {
                    pTempData = pCurrData;
                }
            }
        }
    }

    return pTempData;
}

// setDataToList()
// This function sets the address with the data 
void setDataToList(MipsSimContext *pContext, Data *pData)
{
    Data     *pCurrData = NULL;

    if (pContext->DataList != NULL)
    {
        pCurrData = pContext->DataList;

        if (pCurrData->DataAddress == pData->DataAddress)
        {
            pCurrData->DataDWord = pData->DataDWord;
        }
        else
        {
            while (pCurrData->next != NULL)
            {
                pCurrData = pCurrData->next;
                if (pCurrData->DataAddress == pData->DataAddress)
                {
                    pCurrData->DataDWord = pData->DataDWord;
                    break;
                }
            }
        }
    }
}

// addInstToInstQueue()
// This function adds the instructions to the end of the queue
void addInstToInstQueue(MipsSimContext *pContext, Instruction *pInst)
{
    Instruction *pCurrInst = NULL;
    Instruction *pTempInst = NULL;

    // Allocate memory for Inst
    pTempInst = (Instruction*)malloc(sizeof(Instruction));
    memcpy(pTempInst, pInst, sizeof(Instruction));

    pTempInst->PipelineStage = INSTRUCTION_FETCH;
    pTempInst->next = NULL;

    // Check if the head node is empty 
    if (pContext->InstQueue == NULL)
    {
        pContext->InstQueue = pTempInst;
    }
    else
    {
        // Traverse to the end of the List and add it there
        pCurrInst = pContext->InstQueue;
        while (pCurrInst->next != NULL)
        {
            pCurrInst = pCurrInst->next;
        }
        pCurrInst->next = pTempInst;
    }
}

// moveInstToISStage()
// This function moves the instructions fethed in the previous cycle to 
// IS stage so that they can be added to ROB in the current cycle 
// THis instruction should be the last one in the Instruction queue!
void moveInstToISStage(MipsSimContext *pContext)
{
    Instruction     *pCurrInst = pContext->InstQueue;

    // Traverse through the Queue and move Inst to IS stage 
    if (pCurrInst == NULL)
    {
        // Nothing to be done!
    }
    else
    {
        while (pCurrInst->next != NULL)
        {
            pCurrInst = pCurrInst->next;
        }
        
        // Last instruction in the queue!
        // Just a check that this instruction was in IF stage
        if (pCurrInst->PipelineStage == INSTRUCTION_FETCH)
        {
            pCurrInst->PipelineStage = INSTRUCTION_ISSUE;
        }
    }
}

// IsInstQueueEmpty()
// This function returns true if the Instruction Queue is empty
bool IsInstQueueEmpty(MipsSimContext *pContext)
{
    return (pContext->InstQueue == NULL) ? true : false;
}

// getInstFromList()
// This function returns the first instruction in the IQ thats ready to be issued
// This should be the first instruction in the Instruction Queue
Instruction* getInstToIssue(MipsSimContext *pContext)
{
    Instruction*    pCurrInst = pContext->InstQueue;
    Instruction*    pTempInst = NULL;

    if (pCurrInst != NULL)
    {
        // Return this instruction, just make a check that its in IS stage 
        if (pCurrInst->PipelineStage == INSTRUCTION_ISSUE)
        {
            pTempInst = pCurrInst;
        }
    }

    return pTempInst;
}

// removeInstFromInstQueue()
// This function removes the Inst from IQ and moves it to ROB
void removeInstFromInstQueue(MipsSimContext *pContext, Instruction *pInst)
{
    Instruction*    pCurrInst = pContext->InstQueue;
    Instruction*    pPrevInst = pContext->InstQueue;

    // Check if head node has to be removed 
    if (pCurrInst == pInst)
    {
        pContext->InstQueue = pCurrInst->next;
        pCurrInst->next = NULL;
    }
    else
    {
        // Traverse to the node
        while (pCurrInst->next != NULL)
        {
            pPrevInst = pCurrInst;
            pCurrInst = pCurrInst->next;

            if (pCurrInst == pInst)
            {
                pPrevInst->next = pCurrInst->next;
                pCurrInst->next = NULL;
                break;
            }
        }
    }
}

// getInstFromROBInOrder()
// This function reorders the instruction in ROB in the order from oldest 
// to newest & returns the num of inst in ROB
UINT reorderROBInst(MipsSimContext *pContext, Instruction *ROBInst)
{
    UINT    ROBCount = 0;
    UINT    ROBInstCount = 0;
    UINT    i = 0, j = 0;
    UINT    minInstIssueNum = 0;
    Instruction tempInst;

    // Put all the instructions in ROB in the ROB Instruction Array 
    for (ROBCount = 0; ROBCount < MAX_ROB_ENTRIES; ROBCount++)
    {
        if (pContext->ROB[ROBCount].Busy)
        {
            memcpy(&ROBInst[ROBInstCount++], pContext->ROB[ROBCount].pInst, sizeof(Instruction));
        }
    }

    // Sort the instructions in the ROBInstArray in order of InstIssueNum 
    for (i = 1; i < ROBInstCount; i++)
    {
        j = i;
        while (j > 0)
        {
            if (ROBInst[j].InstIssueNum < ROBInst[j - 1].InstIssueNum)
            {
                memcpy(&tempInst, &ROBInst[j - 1], sizeof(Instruction));
                memcpy(&ROBInst[j - 1], &ROBInst[j], sizeof(Instruction));
                memcpy(&ROBInst[j], &tempInst, sizeof(Instruction));
            }
            else break;
            j--;
        }
    }

    return ROBInstCount;
}

// reorderRSInst()
// This functions reorders RS instructions in order oldest to newest
// and returns the no of Instructions in RS
UINT reorderRSInst(MipsSimContext *pContext, Instruction *RSInst)
{
    UINT    RSCount = 0;
    UINT    RSInstCount = 0;
    UINT    i = 0, j = 0;
    UINT    minInstIssueNum = 0;
    Instruction tempInst;

    // Put all the instructions in ROB in the ROB Instruction Array 
    for (RSCount = 0; RSCount < MAX_RS_ENTRIES; RSCount++)
    {
        if (pContext->RS[RSCount].Busy)
        {
            memcpy(&RSInst[RSInstCount++], pContext->RS[RSCount].pInst, sizeof(Instruction));
        }
    }

    // Sort the instructions in the ROBInstArray in order of InstIssueNum 
    for (i = 1; i < RSInstCount; i++)
    {
        j = i;
        while (j > 0)
        {
            if (RSInst[j].InstIssueNum < RSInst[j - 1].InstIssueNum)
            {
                memcpy(&tempInst, &RSInst[j - 1], sizeof(Instruction));
                memcpy(&RSInst[j - 1], &RSInst[j], sizeof(Instruction));
                memcpy(&RSInst[j], &tempInst, sizeof(Instruction));
            }
            else break;
            j--;
        }
    }

    return RSInstCount;
}

// getROBEntryOldestInst()
// This function returns the oldest issued instruction in the ROB
UINT getROBEntryOldestInst(MipsSimContext *pContext)
{
    Instruction     ROBInst[MAX_RS_ENTRIES];
    UINT            ROBInstCount = 0;
    UINT            count = 0;

    ROBInstCount = reorderROBInst(pContext, ROBInst);

    // Check which ROB has this instruction
    for (count = 0; count < MAX_ROB_ENTRIES; count++)
    {
        if (pContext->ROB[count].Busy && pContext->ROB[count].pInst->InstIssueNum == ROBInst[0].InstIssueNum)
        {
            break;
        }
    }

    return count;
}

// IsROBEmpty()
// This function checks if all the ROB entries are empty 
bool IsROBEmpty(MipsSimContext *pContext)
{
    UINT count = 0;

    for (count = 0; count < MAX_ROB_ENTRIES; count++)
    {
        if (pContext->ROB[count].Busy)
        {
            return false;
        }
    }

    return true;
}

