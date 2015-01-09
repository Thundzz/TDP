CC=gcc
CFLAGS= -W -Wall -O2 -std=c99 -fopenmp
LDFLAGS= -W -Wall -O2 -pthread -fopenmp
OUTPUT = #-DOUTPUT_BOARD
EXEC= life_seq.out life_omp.out life_pthread.out
TESTS= 
LIB=

send:
	scp * formation:TDP5

all: $(EXEC) $(BENCH) $(TESTS)

%.out: %.o $(LIB)
	$(CC)  -o $@ $(LIB) $< $(LDFLAGS) $(OUTPUT)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(OUTPUT)
	
clean:
	rm -rf *.o  *.[oe][0-9]* *~ *# *.out
