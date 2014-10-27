CC=gcc
CFLAGS=-W -Wall -O2
LDFLAGS=
EXEC=driver.out
PERF=perf.out

all: $(PERF)


perf.out: perf.o ddot.o util.o
	$(CC) -o $@ $^ $(LDFLAGS)
	
driver.out: driver.o util.o ddot.o dgemm.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
clean:
	rm -rf *.o *~ *# *.out
