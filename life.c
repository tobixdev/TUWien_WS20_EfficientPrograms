#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "life.h"

FILE *infile;
Celllist *gen0;

char buf1[BUFFER_SIZE][BUFFER_SIZE];
char buf2[BUFFER_SIZE][BUFFER_SIZE];

char (*current)[BUFFER_SIZE][BUFFER_SIZE] = &buf1;
char (*next)[BUFFER_SIZE][BUFFER_SIZE] = &buf2;

Worklist* current_worklist;
Worklist* next_worklist;

void push(Worklist* worklist, char* field) {
  for (int i = 0; i < worklist->current; i++) {
    if (worklist->elements[i] == field) {
      return;
    }
  }
  worklist->current++;
  worklist->elements[worklist->current] = field;
}

char* pop(Worklist* worklist) {
  return worklist->elements[worklist->current--];
}

Celllist *newcell(long x, long y, Celllist *l)
{
  Celllist *c = malloc(sizeof(Celllist));
  /*fprintf(stderr,"newcell x=%ld y=%ld\n",x,y);*/
  c->x = x;
  c->y = y;
  c->next = l;
  return c;
}

int neighbourhood(char* cell)
{
  int n=0;
  n += *(cell - BUFFER_SIZE - 1);
  n += *(cell - BUFFER_SIZE);
  n += *(cell - BUFFER_SIZE + 1);
  n += *(cell - 1);
  n += *(cell) * 9;
  n += *(cell + 1);
  n += *(cell + BUFFER_SIZE - 1);
  n += *(cell + BUFFER_SIZE);
  n += *(cell + BUFFER_SIZE + 1);
  return n;
}

void push_neighbourhood(Worklist* worklist, char* cell)
{
  push(worklist, cell - BUFFER_SIZE - 1);
  push(worklist, cell - BUFFER_SIZE);
  push(worklist, cell - BUFFER_SIZE + 1);
  push(worklist, cell - 1);
  push(worklist, cell);
  push(worklist, cell + 1);
  push(worklist, cell + BUFFER_SIZE - 1);
  push(worklist, cell + BUFFER_SIZE);
  push(worklist, cell + BUFFER_SIZE + 1);
}

void onegeneration()
{
  int i = 0;
  while(current_worklist->current >= 0) {
      char* cell = pop(current_worklist);
      long n = neighbourhood(cell);
      char* cell_new = ((char*) *next) + (cell - ((char*) *current));
      *cell_new = (n == 3) | (n == 11) | (n == 12);
      if ((*cell_new) != (*cell)) {
        i++;
        push_neighbourhood(next_worklist, cell_new);
      }
  }
  char (*h)[BUFFER_SIZE][BUFFER_SIZE] = current;
  current = next;
  next = h;
  
  current_worklist->current = -1;
  Worklist* h_worklist = current_worklist;
  current_worklist = next_worklist;
  next_worklist = h_worklist;
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

  current_worklist = malloc(sizeof(Worklist));
  next_worklist = malloc(sizeof(Worklist));
  current_worklist->current = -1;
  next_worklist->current = -1;
  
  for (Celllist* l = gen0; l; l = l->next){
    (*current)[l->x + OFFSET][l->y + OFFSET] = 1;
    push_neighbourhood(current_worklist, &(*current)[l->x + OFFSET][l->y + OFFSET]);
  }
  freecelllist(gen0);

  for (i=0; i<generations; i++) {
    onegeneration();
  }
  writelife(stdout);
  fprintf(stderr,"%ld cells alive\n", countcells());
  return 0;
}
