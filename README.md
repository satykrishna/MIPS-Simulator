MIPS SIMULATOR
-----------------

MIPS simulator is capable of loading a specified MIPS binary file and outputting the assembly code equivalent or cycle-by-cycle simulation of the MIPS binary code. 

   <br>Part I was to create the disassembler. This program is capable of loading a provided binary file and displaying the MIPS assembly code equivalent along with the binary code. <br>
   <br>Part II is to develop a cycle-by-cycle MIPS simulator <br>

SUPPORTED MIPS INSTRUCTIONS
---------------------------
This simulator supports following instructions
   <li> SW, LW <br>
   <li> J, BEQ, BNE, BGEZ, BGTZ, BLEZ, BLTZ <br>
   <li> ADDI, ADDIU <br>
   <li> BREAK <br>
   <li> SLT, SLTI , SLTU <br>
   <li> SLL, SRL, SRA <br>
   <li> SUB, SUBU, ADD, ADDU <br>
    <li>AND, OR, XOR, NOR <br>
   <li> NOP <br>
   
   
PART 1
------
<b><i><u>INPUT </u></i></b>: Input to the project at this stage is  a binary input file. This file will contain a sequence of 32-bit instruction words which begin at address "600". The final instruction in the sequence of instructions is always BREAK. The data section is followed the BREAK instruction and begins at address "716". <br> <br> Following that is a sequence of 32-bit 2's complement signed integers for the program data up to the end of file. Note that the instruction words always start at address "600". If the instructions occupy beyond address "716", the data region will be followed the BREAK instruction immediately. 
<br><br>
<b><i>OUTPUT </i></b>: The disassembler output file will contain 4 columns of data with each column separated by one space character (' '). Each column is as follows

<li>The binary (e.g., 0's and 1's) string representing the 32-bit data word at that location. For instructions you should split this into six groups of digits to represent different parts of the MIPS instruction word: a group of 6 bits, 4 groups of 5 bits, and a final group of 6 bits.<br>
<li>The address (in decimal) of that location.<br>
<li>The disassembled instruction opcode+other fields, or a signed decimal integer value, depending on whether the current location is after the BREAK instruction.<br>
<li>If you are displaying an instruction, the fourth column should contain the remaining part of the instruction, with each argument separated by a comma and then a space. (", ")<br>

PART2
-----
In this part, I have implemented advanced pipeline using Tomasulo algorithm with out-of-order execution and in-order commit along with a Branch Predictor using the Branch Target Buffer for a processor which executes MIPS32 instructions as defined in Part I of the project. 

The Tomasulo Algorithm is implemented with two simplifications in handling store-load memory dependences: <br>
1.	The memory addresses for loads and stores are generated in order. <br>
2.	When a load is data dependent on an early store (with a matching address), the load stalls until the parent store commits and the data is stored into memory. If a match is found with multiple early stores, the parent is the latest store. <br>

