.KEEP_STATE:
CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused -O2
LDFLAGS = -lm

all: erdos rgg sf-buffer sf sw

erdos: erdos.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

rgg: rgg.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

sf-buffer: sf-buffer.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

sf: sf.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

sw: sw.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)


erdos.o: erdos.c
	$(CC) $(CFLAGS) -c $< -o $@

rgg.o: rgg.c
	$(CC) $(CFLAGS) -c $< -o $@

sf.o: sf.c
	$(CC) $(CFLAGS) -c $< -o $@

sw.o: sw.c
	$(CC) $(CFLAGS) -c $< -o $@

sf-buffer.o: sf-buffer.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o erdos rgg sf sw sf-buffer