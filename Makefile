DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -std=c99 -pedantic -Wall -g

all: nqueens

nqueens: nqueens.o
	@ gcc $(CFLAGS) $(DEFS) -o nqueens $<

nqueens.o: nqueens.c
	@ gcc $(CFLAGS) $(DEFS) -c $<

clean:
	@ rm -rf *.o nqueens

perf:
	@ perf_run.sh

clean-perf:
	@ rm perf_output.txt perf_result.txt