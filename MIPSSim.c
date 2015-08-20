//
//  This is the main soure file for MIPS Dissasembler
//

#include "FuncProto.h"

// Register List Definition
RegList RegLookUpTable =
{
    { 0x0, "R0", 0 },
    { 0x1, "R1", 0 },
    { 0x2, "R2", 0 },
    { 0x3, "R3", 0 },
    { 0x4, "R4", 0 },
    { 0x5, "R5", 0 },
    { 0x6, "R6", 0 },
    { 0x7, "R7", 0 },
    { 0x8, "R8", 0 },
    { 0x9, "R9", 0 },
    { 0xa, "R10", 0 },
    { 0xb, "R11", 0 },
    { 0xc, "R12", 0 },
    { 0xd, "R13", 0 },
    { 0xe, "R14", 0 },
    { 0xf, "R15", 0 },
    { 0x10, "R16", 0 },
    { 0x11, "R17", 0 },
    { 0x12, "R18", 0 },
    { 0x13, "R19", 0 },
    { 0x14, "R20", 0 },
    { 0x15, "R21", 0 },
    { 0x16, "R22", 0 },
    { 0x17, "R23", 0 },
    { 0x18, "R24", 0 },
    { 0x19, "R25", 0 },
    { 0x1a, "R26", 0 },
    { 0x1b, "R27", 0 },
    { 0x1c, "R28", 0 },
    { 0x1d, "R29", 0 },
    { 0x1e, "R30", 0 },
    { 0x1f, "R31", 0 },
    { 0x20, "Error#", 0 }
};

// printArguementUsage()
// This function prints the proper usage of the 
// arguements to be entered by the user
void printArguementUsage()
{
    printf("\n");
    printf("\tMIPSsim inputfilename outputfilename [-Tm:n]\n\n");
    printf("inputfilename   : The file name of the binary input file\n");
    printf("outputfilename  : The file name to which to print the output\n");
    printf("[-Tm:n]         : Optional arguement to specify the start m and end n cycles of simulating trace output.\n");
    printf("\n");
}

// parseArguements()
// This function parses the arguements and fills up 
// the args structure to be used later
bool parseArguements(MipsSimContext *pContext, char *argv[])
{
    int             filenameLength = 0;
    bool            ReturnStatus = true;
    char            *startCycleNum = NULL;
    char            *endCycleNum = NULL;
    UINT            count = 0, i = 0;
    UINT            length = 0;

    do
    {
        //get the file name of the input binary file 
        filenameLength = strlen(argv[1]);
        if (filenameLength > 0)
        {
            pContext->Args.InputFilename = (char*)malloc(sizeof(filenameLength + 1));
            strcpy(pContext->Args.InputFilename, argv[1]);
        }
        else
        {
            printf("Illegal Filename!!\n");
            ReturnStatus = false;
            break;
        }

        // get the filename of the output file 
        filenameLength = strlen(argv[2]);
        if (filenameLength > 0)
        {
            pContext->Args.OutputFilename = (char*)malloc(sizeof(filenameLength + 1));
            strcpy(pContext->Args.OutputFilename, argv[2]);
        }
        else
        {
            printf("Illegal Filename!!\n");
            ReturnStatus = false;
            break;
        }

        if (argv[3] != NULL)
        {
            // get the start and end cycle Num
            for (count = 2; argv[3][count] != ':'; count++)
            {
                length++;
            }
            startCycleNum = (char*)malloc(sizeof(char) * (length + 1));
            for (count = 2; argv[3][count] != ':'; count++)
            {
                startCycleNum[i++] = argv[3][count];
            }
            startCycleNum[i] = '\0';

            i = 0; count++;
            endCycleNum = (char*)malloc(sizeof(char)*(strlen(argv[3] - count) + 1));
            for (count; count <= strlen(argv[3]); count++)
            {
                endCycleNum[i++] = argv[3][count];
            }

            pContext->Args.StartCycleNum = atoi(startCycleNum);
            pContext->Args.EndCycleNum = atoi(endCycleNum);

            if (pContext->Args.StartCycleNum == 0 && pContext->Args.EndCycleNum == 0)
            {
                pContext->Args.PrintFinalState = true;
            }
        }
        else
        {
            pContext->Args.PrintAllCycles = true;
        }
    } while (false);

    if (!ReturnStatus)
    {
        // free up memory 
        if (pContext->Args.InputFilename)
        {
            free(pContext->Args.InputFilename);
            pContext->Args.InputFilename = NULL;
        }

        if (pContext->Args.OutputFilename)
        {
            free(pContext->Args.OutputFilename);
            pContext->Args.OutputFilename = NULL;
        }
    }

    return ReturnStatus;
}

// swapBytes()
// swaps the bytes 0 & 3, bytes 1 & 2 in a DWord
void swapBytes(char *Byte1, char* Byte2)
{
    char temp;

    // Swap Bytes
    temp = *Byte1;
    *Byte1 = *Byte2;
    *Byte2 = temp;
}

// parseOpcode()
// Parses the Opcode of the Instruction and accordingly 
// calls the decode function to create the string
void parseOpcode(MipsSimContext *pContext, Instruction *pInst)
{
    // Get the opcode from Instruction Type 
    pInst->Opcode = (pInst->InstructionDWord.InstDwordValue & OpcodeMask) >> OpcodeShift;
    switch (pInst->Opcode)
    {
    case OPCODE_SPECIAL:
        switch (pInst->InstructionDWord.RType.Function)
        {
        case FUNC_SLL:
            // This could be either SLL or NOP
            // SLL R0, R0, 0 is expressed as NOP, will be checked within the SLL decoding
            decodeSLL(pInst);
            break;
        case FUNC_SRL:
            decodeSRL(pInst);
            break;
        case FUNC_SRA:
            decodeSRA(pInst);
            break;
        case FUNC_BREAK:
            decodeBREAK(pInst);
            pContext->IsBreakInst = true;
            break;
        case FUNC_ADD:
            decodeADD(pInst);
            break;
        case FUNC_ADDU:
            decodeADDU(pInst);
            break;
        case FUNC_SUB:
            decodeSUB(pInst);
            break;
        case FUNC_SUBU:
            decodeSUBU(pInst);
            break;
        case FUNC_AND:
            decodeAND(pInst);
            break;
        case FUNC_OR:
            decodeOR(pInst);
            break;
        case FUNC_XOR:
            decodeXOR(pInst);
            break;
        case FUNC_NOR:
            decodeNOR(pInst);
            break;
        case FUNC_SLT:
            decodeSLT(pInst);
            break;
        case FUNC_SLTU:
            decodeSLTU(pInst);
            break;
        default:
            // should not happen
            break;
        }
        break;
    case OPCODE_REGIMM:
        switch (pInst->InstructionDWord.IType.Rt)
        {
        case RT_BLTZ:
            decodeBLTZ(pInst);
            break;
        case RT_BGEZ:
            decodeBGEZ(pInst);
            break;
        default:
            // should not happen
            break;
        }
        break;
    case OPCODE_J:
        decodeJ(pInst);
        break;
    case OPCODE_BEQ:
        decodeBEQ(pInst);
        break;
    case OPCODE_BNE:
        decodeBNE(pInst);
        break;
    case OPCODE_BLEZ:
        decodeBLEZ(pInst);
        break;
    case OPCODE_BGTZ:
        decodeBGTZ(pInst);
        break;
    case OPCODE_ADDI:
        decodeADDI(pInst);
        break;
    case OPCODE_ADDIU:
        decodeADDIU(pInst);
        break;
    case OPCODE_SLTI:
        decodeSLTI(pInst);
        break;
    case OPCODE_SW:
        decodeSW(pInst);
        break;
    case OPCODE_LW:
        decodeLW(pInst);
        break;
    default:
        // should not happen
        break;
    }
}

// addAllInstToContext()
// This function reads all the Instructions and adds them to the context
// and builds the linked list
void addAllInstToContext(MipsSimContext *pContext)
{

    Instruction     ReadInstruction = { 0 };

    memset(&ReadInstruction, 0, sizeof(Instruction));

    while (true)
    {
        memset(&ReadInstruction, 0, sizeof(Instruction));
        
        // Read DWord from the Input file, build the Instruction structure  
        ReadInstruction.InstructionDWord.InstDwordValue = readUnsignedDWordFile(pContext->FileHandle.InputFileHandle);
        pContext->NoOfDWordsRead++;

        // Parse Opcode
        parseOpcode(pContext, &ReadInstruction);

        // Add memory address of the Instruction
        ReadInstruction.InstAddress = pContext->CurrentAddress;
        pContext->CurrentAddress += 4;
        ReadInstruction.next = NULL;

        // Add the instruction to the List 
        addInstToList(pContext, &ReadInstruction);
        pContext->InstructionCount++;

        // If EOF hit , set the flag
        if (pContext->NoOfDWordsRead >= (pContext->InputFileLength >> 2))
        {
            pContext->IsEndOfFileHit = true;
        }

        // Exit the loop if break Inst hit or end of file 
        if (pContext->IsBreakInst)
        {
            break;
        }
    }
}

// addAllDataToContext()
// This function reads all the data segment and adds them to the context
// and builds the data linked list
void addAllDataToContext(MipsSimContext *pContext)
{
    INT     ReadDWord = 0;

    while (true)
    {
        // Read DWord from the Input file 
        ReadDWord = readSignedDWordFile(pContext->FileHandle.InputFileHandle);
        pContext->NoOfDWordsRead++;

        // Start Adding the Data DWords if hit the Data Segment Start
        if (pContext->CurrentAddress >= DATA_START_ADDRESS)
        {
            addDataToList(pContext, ReadDWord);
            pContext->DataCount++;
        }

        // Increment the address 
        pContext->CurrentAddress += 4;

        // Exit of EOF is hit 
        if (pContext->NoOfDWordsRead >= (pContext->InputFileLength >> 2))
        {
            break;
        }
    }
}

// parseInputFile()
// This function parses the input file and stores all the instructions 
void parseInputFile(MipsSimContext *pContext)
{
    // Read all the Instructions and add them to the context 
    addAllInstToContext(pContext);

    // If EOF hit , done with file reading 
    if (!pContext->IsEndOfFileHit)
    {
        // Add all the data bytes to the file
        addAllDataToContext(pContext);
    }
}

// Main function !!
int main(int argc, char *argv[])
{
    int                 ReturnStatus = 1;
    MipsSimContext      Context;

    // Set all the context variables 
    // Set the Start memory address in the file for instruction fetch 
    Context.CurrentAddress = INST_START_ADDRESS;
    Context.currentBufferPos = 0;
    Context.InputFileLength = 0;
    Context.IsBreakInst = false;
    Context.IsEndOfFileHit = false;
    Context.NoOfDWordsRead = 0;
    memset(Context.Buffer, 0, sizeof(Context.Buffer));
    Context.InstructionList = NULL;
    Context.InstructionCount = 0;
    Context.DataList = NULL;
    Context.DataCount = 0;
    Context.CurrentPC = INST_START_ADDRESS;
    Context.CurrentCycle = 0;
    Context.InstQueue = NULL;
    memset(Context.ROB, 0, sizeof(Context.ROB));
    memset(Context.RS, 0, sizeof(Context.RS));
    memset(Context.RegisterStat, 0, sizeof(Context.RegisterStat));
    Context.CurrentInstIssueNum = 0;
    Context.BTB = NULL;
    Context.RemoveInstFromQueue = false;
    Context.Args.PrintFinalState = false;
    Context.Args.PrintAllCycles = false;

    do
    {
        // Check if the user has correctly entered the arguements
        if (argc < 3 || argc > 4)
        {
            printArguementUsage();
            break;
        }

        // Parse the program arguements and fill the parameters
        if (!parseArguements(&Context, argv))
        {
            printf("Recheck Arguements!\n");
            printArguementUsage();
            ReturnStatus = -1;
            break;
        }

        // checks Input and output files, creates the output file if not present
        if (!verifyInOutFiles(&Context))
        {
            printf("Not able to read Input file or write to output file!\n");
            ReturnStatus = -1;
            break;
        }

        // SIMULATE TOMASULO ALGORITHM !!!!!

        // Parse the input binary file to get all the instructions and Data added to the Context
        parseInputFile(&Context);

        // Start Simulation with Tomasulo Algorithm
        simulateExTomasulo(&Context);

        // close files 
        closeFiles(&Context);


    } while (false);

    return ReturnStatus;
}

