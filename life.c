#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "life.h"
#define BUFFER_SIZE 2300
// this is to support negative values
#define OFFSET 200

FILE *infile;
Celllist *gen0;

char buf1[BUFFER_SIZE][BUFFER_SIZE];
char buf2[BUFFER_SIZE][BUFFER_SIZE];

char (*current)[BUFFER_SIZE][BUFFER_SIZE] = &buf1;
char (*next)[BUFFER_SIZE][BUFFER_SIZE] = &buf2;

Celllist *newcell(long x, long y, Celllist *l)
{
  Celllist *c = malloc(sizeof(Celllist));
  /*fprintf(stderr,"newcell x=%ld y=%ld\n",x,y);*/
  c->x = x;
  c->y = y;
  c->next = l;
  return c;
}

int neighbourhood(long x, long y)
{
  int n=0;
  n += (*current)[x-1][y-1];
  n += (*current)[x-1][y];
  n += (*current)[x-1][y+1];
  n += (*current)[x][y-1];
  n += (*current)[x][y] * 9;
  n += (*current)[x][y+1];
  n += (*current)[x+1][y-1];
  n += (*current)[x+1][y];
  n += (*current)[x+1][y+1];
  return n;
}

void onegeneration()
{
  for (long i = 1; i < BUFFER_SIZE - 1; i++) {
    for (long j = 1; j < BUFFER_SIZE - 1; j++) {
      long n = neighbourhood(i, j);
      (*next)[i][j] = (n == 3) | (n == 11) | (n == 12);
    }
  }
  char (*h)[BUFFER_SIZE][BUFFER_SIZE] = current;
  current = next;
  next = h;
}

void freecelllist(Celllist *l)
{
  while (l) {
    Celllist *old = l;
    l = l->next;
    free(old);
  }
}

Celllist *readlife(FILE *f)
{
  infile = f;
  if (yyparse() != 0) {
    fprintf(stderr, "syntax error");
    exit(1);
  }
  return gen0;
}

void writelife(FILE *f)
{
  for(long i = 0; i < BUFFER_SIZE; i++) {
    for(long j = 0; j < BUFFER_SIZE; j++) {
      if ((*current)[i][j]) {
        fprintf(f, "%ld %ld\n", i - OFFSET, j - OFFSET);
      }
    }
  }
}

long countcells()
{
  long c = 0;
  for(long i = 0; i < BUFFER_SIZE; i++) {
    for(long j = 0; j < BUFFER_SIZE; j++) {
      c += (*current)[i][j];
    }
  }
  return c;
}

int main(int argc, char **argv)
{
  long generations;
  long i;
  char *endptr;

  if (argc!=2) {
    fprintf(stderr, "Usage: %s #generations <startfile | sort >endfile\n", argv[0]);
    exit(1);
  }
  generations = strtol(argv[1], &endptr, 10);
  if (*endptr != '\0') {
    fprintf(stderr, "\"%s\" not a valid generation count\n", argv[3]);
    exit(1);
  }
  readlife(stdin);
  
  for (Celllist* l = gen0; l; l = l->next){
    (*current)[l->x + OFFSET][l->y + OFFSET] = 1;
  }
  freecelllist(gen0);

  for (i=0; i<generations; i++) {
    onegeneration();
  }
  writelife(stdout);
  fprintf(stderr,"%ld cells alive\n", countcells());
  return 0;
}
