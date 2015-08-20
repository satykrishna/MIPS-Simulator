/*
 * btb.h
 *
 *  Created on: 03-Nov-2014
 *      Author: Satya
 */

#ifndef BTB_H_
#define BTB_H_

#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "InstParser.h"

#define BTB_MAX_SIZE 16

typedef enum _PredictorState
{
    NOT_SET,
    BRANCH_TAKEN,
    BRANCH_NOT_TAKEN,
}PredictorState;

/* BTB table*/
typedef struct _BTBEntry
{
    UINT    InstAddress;
    UINT    TargetAddress;
    UINT    PredState;
    struct  _BTBEntry *next;
    UINT    HitRate; /*checks the condition of LRU policy for btb*/
}BTBEntry, *PBTBEntry;

#endif /* BTB_H_ */
