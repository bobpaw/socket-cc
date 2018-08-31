CC = g++
CFLAGS = -g -Og -Wall -pedantic

targets = client server showip

ALL: $(targets)
.PHONY: clean clean-bak

showip: showip.cc cmdline-showip.c
	$(CC) $(CFLAGS) $^ -o $@

cmdline-showip.c: showip.ggo
	gengetopt < $<

%: %.cc
	$(CC) $(CFLAGS) $< -o $@

clean-bak:
	rm -f *~ \#*#

clean: clean-bak
	rm -f $(targets) *.o
