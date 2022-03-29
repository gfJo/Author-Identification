CC = clang
CFLAGS = -Wall -Werror -Wpedantic -Wextra
COMMON_OBJS = bf.o bv.o ht.o parser.o pq.o speck.o text.o node.o

all: identify

identify: identify.o $(COMMON_OBJS)
	$(CC) -o $@ $^ -lm

identify.o: identify.c
	$(CC) $(CFLAGS) -c identify.c

bf.o:bf.c
	$(CC) $(CFLAGS) -c bf.c

pq.o: pq.c
	$(CC) $(CFLAGS) -c pq.c

bv.o: bv.c
	$(CC) $(CFLAGS) -c bv.c

ht.o: ht.c
	$(CC) $(CFLAGS) -c ht.c

parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c

speck.o: speck.c
	$(CC) $(CFLAGS) -c speck.c

text.o: text.c
	$(CC) $(CFLAGS) -c text.c

node.o: node.c
	$(CC) $(CFLAGS) -c node.c

format:
	clang-format -i -style=file *.c *.h

clean:
	rm -f *.o identify

