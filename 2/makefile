CC=mpicc
CFLAGS= -W -Wall -O2 -std=c99
LDFLAGS= -W -Wall -O2 -lm
EXEC=main.out simulation.out
BENCH=
TEST=dtcalc_test.out

all: $(TEST) $(BENCH) $(EXEC)

test: $(TEST)

send:
	scp * formation:TDP/2

simu:
	mpiexec -np 2 simulation.out 

plot:
	gnuplot plot.plt

dtcalc_test.out: particule.o dtcalc.o dtcalc_test.o
	$(CC) -o $@ $^ $(LDFLAGS)

simulation.out: particule.o simulation.o dtcalc.o
	$(CC) -o $@ $^ $(LDFLAGS)
	
main.out: particule.o main.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
clean:
	rm -rf *.o  *.[oe][0-9]* *~ *# *.out
