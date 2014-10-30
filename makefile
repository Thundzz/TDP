CC=gcc
CFLAGS= -W -Wall -O2 -DBLOCK_SIZE=100
LDFLAGS= -W -Wall -O2
EXEC=driver.out
BENCH=benchmark_ddot.out benchmark_dgemm.out

all: $(EXEC) $(BENCH)

send:
	scp * formation:TDP/


recup:
	scp formation:TDP/*.txt .

benchmark_ddot.out: benchmark_ddot.o perf.o ddot.o util.o
	$(CC) -o $@ $^ $(LDFLAGS)

benchmark_dgemm.out: benchmark_dgemm.o perf.o dgemm.o util.o
	$(CC) -o $@ $^ $(LDFLAGS)

	
driver.out: driver.o util.o ddot.o dgemm.o daxpy.o dgemv.o dger.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
clean:
	rm -rf *.o  *.[oe][0-9]* *~ *# *.out
