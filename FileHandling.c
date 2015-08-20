//
// This file contains all the functions for Input 
// and Output file handling 
//

#include "FuncProto.h"

// verifyInOutFiles()
// This function verifies if the input file is readable
// and the output fils is writable
bool verifyInOutFiles(MipsSimContext *pContext)
{
    bool    ReturnStatus = true;

    do
    {
        // Check if we can read from the input file 
        pContext->FileHandle.InputFileHandle = fopen(pContext->Args.InputFilename, "rb");
        if (pContext->FileHandle.InputFileHandle == NULL)
        {
            printf("Unable to open Input file\n");
            ReturnStatus = false;
            break;
        }

        // Get the length of the input file in Bytes
        fseek(pContext->FileHandle.InputFileHandle, 0, SEEK_END);
        pContext->InputFileLength = (UINT)ftell(pContext->FileHandle.InputFileHandle);
        fseek(pContext->FileHandle.InputFileHandle, 0, SEEK_SET);

        // Check if Output file is writable
        pContext->FileHandle.OutputFileHandle = fopen(pContext->Args.OutputFilename, "wt");
        if (pContext->FileHandle.OutputFileHandle == NULL)
        {
            printf("Cannot write to the Output file!\n");
            ReturnStatus = false;
            break;
        }

    } while (false);

    if (!ReturnStatus)
    {
        // Close any opened files 
        closeFiles(pContext);
    }

    return ReturnStatus;
}

// readUnsignedDWordFile()
// This function reads a unsigned DWORD from am opened file
UINT readUnsignedDWordFile(FILE *fp)
{
    union _DWord ReadDWord;

    // As MIPS 32 is big Endian , we have to read bytes and then swap them to get the DWORD
    if (fread(&ReadDWord.Byte, sizeof(char), 4, fp))
    {
        // Read from file successful ! 
        // swap buffers 
        swapBytes(&ReadDWord.Byte[0], &ReadDWord.Byte[3]);
        swapBytes(&ReadDWord.Byte[1], &ReadDWord.Byte[2]);
    }

    return ReadDWord.UnsignedDwordValue;
}

// readSignedDWordFile()
// This function reads a signed DWORD from am opened file
INT readSignedDWordFile(FILE *fp)
{
    union _DWord ReadDWord;

    // As MIPS 32 is big Endian , we have to read bytes and then swap them to get the DWORD
    if (fread(&ReadDWord.Byte, sizeof(char), 4, fp))
    {
        // Read from file successful ! 
        // swap buffers 
        swapBytes(&ReadDWord.Byte[0], &ReadDWord.Byte[3]);
        swapBytes(&ReadDWord.Byte[1], &ReadDWord.Byte[2]);
    }

    return ReadDWord.SignedDWordValue;
}


// closeFiles()
// This functions closes the input or output 
// files if they are opened
void closeFiles(MipsSimContext *pContext)
{
    // Close both Input and Output files if they are opened
    if (pContext->FileHandle.InputFileHandle)
    {
        fclose(pContext->FileHandle.InputFileHandle);
    }

    if (pContext->FileHandle.OutputFileHandle)
    {
        fclose(pContext->FileHandle.OutputFileHandle);
    }
}

void printOutputFile(MipsSimContext *pContext)
{
    Instruction     *pInst = NULL;
    UINT            count = 0;
    Data            *pData = NULL;
    UINT            DataCount = 0;
    Instruction     ROBInst[MAX_ROB_ENTRIES];
    Instruction     RSInst[MAX_RS_ENTRIES];
    UINT            ROBInstCount = 0, RSInstCount = 0;
    BTBEntry        *currBTBEntry = pContext->BTB;

    // From the ROB and RS, pick the instructions in order of oldest to newest and 
    // put them in the array! 
    ROBInstCount = reorderROBInst(pContext, ROBInst);
    RSInstCount = reorderRSInst(pContext, RSInst);

    // first create the buffer to write 
    memset(pContext->Buffer, 0, sizeof(pContext->Buffer));
    pContext->currentBufferPos = 0;

    if (pContext->Args.PrintFinalState || pContext->Args.PrintAllCycles ||
        (pContext->CurrentCycle >= (UINT)pContext->Args.StartCycleNum && pContext->CurrentCycle <= (UINT)pContext->Args.EndCycleNum))
    {
        // Add cycle No 
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "Cycle <%d>:\n", pContext->CurrentCycle);

        // Add Instruction Queue
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "IQ:\n");
        pInst = pContext->InstQueue;
        while (pInst)
        {
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "[");
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%s", pInst->InstString);
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "]\n");
            pInst = pInst->next;
        }

        // Add Instructions in the RS 
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "RS:\n");
        for (count = 0; count < RSInstCount; count++)
        {
            pInst = &RSInst[count];
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "[");
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%s", pInst->InstString);
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "]\n");
        }

        // Add Instructions in the ROB
        // Instructions in the ROB should be printed in the order from oldest to newest 
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "ROB:\n");
        for (count = 0; count < ROBInstCount; count++)
        {
            pInst = &ROBInst[count];
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "[");
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%s", pInst->InstString);
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "]\n");
        }

        // Add BTB Data 
        count = 1;
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "BTB:\n");
        while (currBTBEntry != NULL)
        {
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "[");
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "Entry %d", count++);
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "]:<");
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%d,", currBTBEntry->InstAddress);
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%d,", currBTBEntry->TargetAddress);
            if (currBTBEntry->PredState == NOT_SET)
            {
                pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "NotSet");
            }
            else
            {
                if (currBTBEntry->PredState == BRANCH_TAKEN)
                {
                    pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "1");
                }
                else
                {
                    pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "0");
                }
            }

            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], ">\n");

            currBTBEntry = currBTBEntry->next;
        }

        // Add Register Values 
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "Registers:\n");
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "R00:    ");
        for (count = 0; count < 7; count++)
        {
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%-8d", RegLookUpTable[count].RegValue);
        }
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%d", RegLookUpTable[7].RegValue);
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "\n");
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "R08:    ");
        for (count = 8; count < 15; count++)
        {
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%-8d", RegLookUpTable[count].RegValue);
        }
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%d", RegLookUpTable[15].RegValue);
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "\n");
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "R16:    ");
        for (count = 16; count < 23; count++)
        {
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%-8d", RegLookUpTable[count].RegValue);
        }
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%d", RegLookUpTable[23].RegValue);
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "\n");
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "R24:    ");
        for (count = 24; count < 31; count++)
        {
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%-8d", RegLookUpTable[count].RegValue);
        }
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%d", RegLookUpTable[31].RegValue);
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "\n");

        // Add Data Segement values 
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "Data Segment:\n");
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "716:    ");
        pData = pContext->DataList;
        while (pData)
        {
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%-8d", pData->DataDWord);
            DataCount++;

            if (DataCount == 9)
            {
                break;
            }
            pData = pData->next;
        }
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%d", pData->next->DataDWord);
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "\n");

        // Write the buffer to the file 
        fwrite(pContext->Buffer, pContext->currentBufferPos, 1, pContext->FileHandle.OutputFileHandle);
    }
}

