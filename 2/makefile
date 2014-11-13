CC=mpicc
CFLAGS= -W -Wall -O2 -std=c99
LDFLAGS= -W -Wall -O2 -lm
EXEC=main.out simulation.out
BENCH=
TEST=

all: $(TEST) $(BENCH) $(EXEC)

test: $(TEST)

send:
	scp * formation:TDP/2

simu:
	mpiexec -np 2 simulation.out 

plot:
	gnuplot plot.plt

simulation.out: particule.o simulation.o dtcalc.o
	$(CC) -o $@ $^ $(LDFLAGS)
	
main.out: particule.o main.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
clean:
	rm -rf *.o  *.[oe][0-9]* *~ *# *.out *.gif datafile* particles*.plt
