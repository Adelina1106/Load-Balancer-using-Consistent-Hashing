CC=gcc
CFLAGS=-std=c99 -Wall -Wextra
CDLL=CircularDoublyLinkedList
HASHTABLE=Hashtable
LL=LinkedList
LOAD=load_balancer
SERVER=server

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(CDLL).o $(HASHTABLE).o $(LL).o
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(CDLL).o: $(CDLL).c $(CDLL).h
	$(CC) $(CFLAGS) $^ -c

$(HASHTABLE).o: $(HASHTABLE).c $(HASHTABLE).h
	$(CC) $(CFLAGS) $^ -c

$(LL).o: $(LL).c $(LL).h
	$(CC) $(CFLAGS) $^ -c

clean:
	rm -f *.o tema2 *.h.gch

.PHONY: pack clean run
