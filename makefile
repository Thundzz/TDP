CC=gcc
CFLAGS=-W -Wall
LDFLAGS=
EXEC=driver.out


all: $(EXEC)

driver.out: driver.o util.o ddot.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
clean:
	rm -rf *.o *~ *#
