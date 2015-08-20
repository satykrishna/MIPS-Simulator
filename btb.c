/*
 * btb.c
 *
 *  Created on: 03-Nov-2014
 *      Author: Satya
 *      Details: Implement Branch history table
 */

#include "btb.h"
#include "MIPSSim.h"
#include "FuncProto.h"

/*Checks if instruction is present in BTB, if so return the details of that particular branch instruction*/

BTBEntry* checkInstructionInBTB(MipsSimContext *pContext, UINT InstAddress)
{
    BTBEntry *curr = pContext->BTB;

    while (curr != NULL)
    {
        if (curr->InstAddress == InstAddress)
        {
            break;
        }
        curr = curr->next;
    }

    return curr;
}

/*Get size of the current BTB */

UINT getSizeofBTB(MipsSimContext *pContext)
{
    UINT        count   = 0;
    BTBEntry    *curr   = pContext->BTB;

    while (curr != NULL)
    {
        curr = curr->next;
        count++;
    }

    return count;
}

/* Find Least Recently used Branch*/

BTBEntry* findLRUEntry(MipsSimContext *pContext)
{
    UINT        min     = 1000;
    BTBEntry    *curr   = pContext->BTB;
    BTBEntry    *lru    = NULL;

    while (curr != NULL)
    {
        if (curr->HitRate < min)
        {
            min = curr->HitRate;
            lru = curr;
        }
        curr = curr->next;
    }

    return lru;
}


/* Insert a new branch entry in to BTB under following conditions
 * if current size of BTB is greater than 16 then it replaces the least recently used BTB entry
 * else a new record is added to BTB
 */


void insertNewPredAtISStage(MipsSimContext *pContext, BTBEntry *newEntry)
{
    BTBEntry    *leastUsed  = NULL;
    BTBEntry    *branch     = NULL;
    BTBEntry    *curr       = NULL;

    if (pContext->BTB == NULL)
    {
        pContext->BTB = newEntry;
    }
    else
    {
        if (getSizeofBTB(pContext) >= BTB_MAX_SIZE)
        {
            /* Implement LRU policy in the BTB*/
            leastUsed = findLRUEntry(pContext);
            leastUsed->HitRate = 1;
            leastUsed->InstAddress = newEntry->InstAddress;
            leastUsed->TargetAddress = newEntry->TargetAddress;
            leastUsed->PredState = newEntry->PredState;
            free(newEntry);
        }
        else
        {
            curr = pContext->BTB;
            while (curr->next != NULL)
            {
                curr = curr->next;
            }
            curr->next = newEntry;
        }
    }
}

/* Fetch the next instruction distance from the current instruction which is
 * at relative distance of 4 or at relative distance of (target Instruction Address- current
 * Instruction Address)*/

UINT getNextPCAddress(MipsSimContext *pContext, UINT InstAddress)
{
    BTBEntry    *branchDetails  = checkInstructionInBTB(pContext, InstAddress);
    UINT        NextPCAddress   = InstAddress;
    Instruction *pInst = getInstFromList(pContext, InstAddress);
    BTBEntry    *pBTBEntry = NULL;

    if (branchDetails != NULL)
    {
        // Branch entry found in BTB!!
        branchDetails->HitRate++;

        if (branchDetails->PredState == BRANCH_TAKEN)
        {
            // Pred State is Taken
            NextPCAddress = branchDetails->TargetAddress;
            pInst->BranchPred = BRANCH_TAKEN;
        }
        else
        {
            // Either the Pred State is Not Taken or is not set
            // Move to the next PC 
            NextPCAddress += 4;
            pInst->BranchPred = BRANCH_NOT_TAKEN;
        }
    }
    else
    {
        // Add a new entry in the BTB!
        // if Branch or Jum Instruction, add an entry in BTB if not present
        if (pInst->IsJumpInst || pInst->IsBranchInst)
        {
            if (checkInstructionInBTB(pContext, pInst->InstAddress) == NULL)
            {
                // Add entry in BTB
                pBTBEntry = (BTBEntry*)malloc(sizeof(BTBEntry));
                pBTBEntry->InstAddress = pInst->InstAddress;
                if (pInst->IsJumpInst)
                {
                    pBTBEntry->TargetAddress = pInst->InstructionDWord.JType.Target * 4;
                }
                else
                {
                    pBTBEntry->TargetAddress = pInst->InstAddress + 4 + pInst->InstructionDWord.IType.SignedImmediate * 4;
                }
                pBTBEntry->PredState = NOT_SET;
                pBTBEntry->HitRate = 1;
                pBTBEntry->next = NULL;
                insertNewPredAtISStage(pContext, pBTBEntry);

                pInst->BranchPred = BRANCH_NOT_TAKEN;
            }
        }

        NextPCAddress += 4;
    }

    return NextPCAddress;
}

void updatePredAtEXStage(MipsSimContext *pContext, UINT InstAddress, UINT PredState) 
{
    BTBEntry    *pBTBEntry = NULL;

    // Get the Instruction from the BTB table 
    pBTBEntry = checkInstructionInBTB(pContext, InstAddress);
    pBTBEntry->PredState = PredState;
}

