CC=gcc
LD=gcc
CFLAGS=-Wall -O2
LIBS=-lportaudio -lm

tuner: playtone.o tuner.o
	$(LD) $^ $(LIBS) -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f tuner *.o
