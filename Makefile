CC = g++
MAKE_CFLAGS = $(CFLAGS) -g -Og -Wall -pedantic -pthread

targets = client server showip

ALL: $(targets)
.PHONY: clean clean-bak ALL

showip: showip.cc cmdline-showip.c
	$(CC) $(MAKE_CFLAGS) $^ -o $@

cmdline-showip.c: showip.ggo
	gengetopt < $<

client: client.o socket.o
	$(CC) $(MAKE_CFLAGS) $^ -o $@

server: server.o socket.o
	$(CC) $(MAKE_CFLAGS) $^ -o $@

%.o: %.cc %.h
	$(CC) $(MAKE_CFLAGS) -c $< -o $@

clean-bak:
	rm -f *~ \#*#

clean:
	rm -f $(targets) *.o

clean-cmdline:
	rm -f cmdline-showip.c cmdline-showip.h

full-clean: clean clean-bak clean-cmdline
