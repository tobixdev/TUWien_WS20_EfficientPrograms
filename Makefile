CC=gcc
CFLAGS=-g -O -Wall
LDFLAGS=-g
SOURCES=Makefile life.c life.h readlife.y showlife showlife.ps f0.l
GEN=f0500.l f1000.l f1500.l f2000.l f2500.l f3000.l

life: life.o readlife.tab.o

readlife.tab.o: readlife.tab.c

readlife.tab.c: readlife.y
	bison readlife.y

clean:
	rm -rf life *.o readlife.tab.c effizienz-aufgabe07

perf:
	./perf_run.sh

test:
	life 500 <./assignment/f0.l |sort |diff - ./assignment/f0500.l
	life 3000 <./assignment/f0.l |sort |diff - ./assignment/f3000.l