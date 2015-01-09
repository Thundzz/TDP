CC=mpicc
CFLAGS= -W -Wall -O2 -std=c99 
LDFLAGS= -W -Wall -O2 -pthread
EXEC=
TESTS= driver.out driver_mpi.out

# Uncomment for verbose tests.
VERB = #-DVERBOSE


LIB=dscal.o dgemm.o dger.o util.o lu_seq.o ddot.o

all: $(EXEC) $(BENCH) $(TESTS)

%.out: %.o $(LIB)
	$(CC)  -o $@ $(LIB) $< $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(VERB)
	
clean:
	rm -rf *.o  *.[oe][0-9]* *~ *# *.out
