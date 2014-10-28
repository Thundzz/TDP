CC=gcc
CFLAGS= -W -Wall -O2
LDFLAGS= -W -Wall -O2
EXEC=driver.out
BENCH=benchmark_ddot.out benchmark_dgemm.out


all: $(EXEC) $(BENCH)

benchmark_ddot.out: benchmark_ddot.o perf.o ddot.o util.o
	$(CC) -o $@ $^ $(LDFLAGS)

benchmark_dgemm.out: benchmark_dgemm.o perf.o dgemm.o util.o
	$(CC) -o $@ $^ $(LDFLAGS)

	
driver.out: driver.o util.o ddot.o dgemm.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
clean:
	rm -rf *.o *~ *# *.out
