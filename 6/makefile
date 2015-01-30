CC=mpicc
CFLAGS= -W -Wall -O2 -std=c99 -fopenmp
LDFLAGS= -W -Wall -O2 -pthread -fopenmp -lm
OUTPUT = #-DOUTPUT_BOARD
EXEC= life_seq.out life_omp.out life_pthread.out 
MPI_EXECS=  life_mpi_sync.out life_mpi_async.out life_mpi_persist.out
TESTS= 
LIB=


all: $(EXEC) $(BENCH) $(TESTS) $(MPI_EXECS)

send:
	scp * formation:TDP5



%.out: %.o $(LIB)
	$(CC)  -o $@ $(LIB) $< $(LDFLAGS) $(OUTPUT)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(OUTPUT)
	
clean:
	rm -rf *.o  *.[oe][0-9]* *~ *# *.out
