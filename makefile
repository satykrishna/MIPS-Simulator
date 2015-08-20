all: MIPSSim

MIPSSim: MIPSSim.o FileHandling.o DecodeInst.o btb.o ListHandling.o SimTomasulo.o 
	gcc MIPSSim.o SimTomasulo.o FileHandling.o DecodeInst.o ListHandling.o btb.o -o MIPSSim 

MIPSSim.o: MIPSSim.c 
	gcc -c MIPSSim.c

FileHandling.o: FileHandling.c 
	gcc -c FileHandling.c 

DecodeInst.o: DecodeInst.c 
	gcc -c DecodeInst.c 

btb.o: btb.c
	gcc -c btb.c

SimTomasulo.o: SimTomasulo.c
	gcc -c SimTomasulo.c

ListHandling.o: ListHandling.c
	gcc -c ListHandling.c

clean: 
	rm -rf *.o MIPSSim

