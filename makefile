CC = gcc
CFLAGS = -g -Wall
OBJECTS = httpServer httpClient
all: $(OBJECTS)
TODELETE = *.o

$(objects): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f $(TODELETE)