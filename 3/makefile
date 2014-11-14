CC=gcc
CFLAGS= -W -Wall -g -O2 -std=c99
LDFLAGS= -W -Wall -g -O2 -lm
EXEC=
BENCH=
TEST=test_matrix.out

all: $(TEST) $(BENCH) $(EXEC)

test: $(TEST)

send:
	scp * formation:TDP/3

test_%.out: test_%.o %.o matrix.o
	$(CC) -o $@ $^ $(LDFLAGS)


%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
clean:
	rm -rf *.o  *.[oe][0-9]* *~ *# *.out
