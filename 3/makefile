CC=mpicc
CFLAGS= -W -Wall -g -O2 -std=c99
LDFLAGS= -W -Wall -g -O2 -lm
LIBFILES= matrix.o grid.o
EXEC=main.out
BENCH=
TEST=test_matrix.out test_dgemm.out test_scatter_gather.out test_comm.out

all: $(TEST) $(BENCH) $(EXEC)

test: $(TEST)

send:
	scp * formation:TDP/3

%.out: %.o $(LIBFILES)
	$(CC) -o $@ $^ $(LDFLAGS)

test_%.out: test_%.o %.o $(LIBFILES)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
clean:
	rm -rf *.o  *.[oe][0-9]* *~ *# *.out *.dat *.gp *.plt *.png

timeclean:
	rm -rf *time.dat
