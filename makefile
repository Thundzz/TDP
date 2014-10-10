CC=gcc
CFLAGS=-W -Wall
LDFLAGS=
EXEC=driver.out
PERF=perf.out

all: $(EXEC)


perf.out: perf.o ddot.o util.o
	$(CC) -o $@ $^ $(LDFLAGS)
	
driver.out: driver.o util.o ddot.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
clean:
	rm -rf *.o *~ *# *.out
