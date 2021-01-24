#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "life.h"

FILE *infile;
Celllist *gen0;

Buffer* current;
Buffer* next;

Worklist* current_worklist;
Worklist* next_worklist;

void alive(char* field) {
  *field = 1;
}

void dead(char* field) {
  *field = 0;
}

void kill(char* field) {
  *field = 0;
  push(next_worklist, field);
}

void resurrect(char* field) {
  *field = 1;
  push(next_worklist, field);
}

void (*dispatch[18])(char*) = {
  dead, // dead  + 0 neigh
  kill, // alive + 0 neigh
  dead, // dead  + 1 neigh
  kill, // alive + 1 neigh
  dead, // dead  + 2 neigh
  alive, // alive + 2 neigh
  resurrect, // dead  + 3 neigh
  alive, // alive + 3 neigh
  dead, // dead  + 4 neigh
  kill, // alive + 4 neigh
  dead, // dead  + 5 neigh
  kill, // alive + 5 neigh
  dead, // dead  + 6 neigh
  kill, // alive + 6 neigh
  dead, // dead  + 7 neigh
  kill, // alive + 7 neigh
  dead, // dead  + 8 neigh
  kill, // alive + 8 neigh
};

/**
 * Things to try:
 * - Remove branchless "optimization". (compare afterwards). Probably hurts more than it helps. 
 * - Play with additional information in cell. e.g. neighbour count.
 * -- Store if a cell is active or not active in a single byte -> no 2nd buffer
 * - Improve hash set.
 * -- https://www.sebastiansylvan.com/post/robin-hood-hashing-should-be-your-default-hash-table-implementation/
 * -- advanced: https://probablydance.com/2014/05/03/i-wrote-a-fast-hash-table/
 * - advanced: Generate code for each state (encoded neighbourcount) and jump to it
 * - Low level optimierungen
 * - Execute original program on g0
 * - Measure memory size
 * - Come up with a better hash. Currently we have a long probings.
 * - HashLife: https://en.wikipedia.org/wiki/Hashlife
 * - .....
 * 
 * Things to beautify: 
 * - Remove linked list creation.
 */

size_t inline hash(char* field) {
  // NOTE: WORKLIST_SIZE has to be a power of two
  return ((size_t)field) & (WORKLIST_SIZE - 1);
}

void push(Worklist* worklist, char* field) {
  int probe_start = hash(field);

  for (int i = probe_start; i < WORKLIST_SIZE; i++) {
    if (worklist->elements[i] == NULL) {
      worklist->elements[i] = field;
      return;
    } else if (worklist->elements[i] == field) {
      return;
    }
  }

  // if the end of the list is reached start at the beginning
  for (int i = 0; i < probe_start; i++) {
    if (worklist->elements[i] == NULL) {
      worklist->elements[i] = field;
      return;
    } else if (worklist->elements[i] == field) {
      return;
    }
  }

  printf("Worklist is full. Aborting program.");
  exit(-1);
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
  n += *(cell + 1);
  n += *(cell + BUFFER_SIZE - 1);
  n += *(cell + BUFFER_SIZE);
  n += *(cell + BUFFER_SIZE + 1);
  return n;
}

void onegeneration()
{
  for (int i = 0; i < WORKLIST_SIZE; i++)
  {
    char* center = current_worklist->elements[i];
    if (center != NULL) {
      for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
          char* cell = center + BUFFER_SIZE * i + j;
          char* cell_new = ((char*) next->cells) + (cell - ((char*) current->cells));
          long n = neighbourhood(cell);
          void (*fn)(char*) = dispatch[(size_t) (n << 1) | *cell];
          fn(cell_new);
        }
      }
    }
    current_worklist->elements[i] = NULL;
  }

  Buffer* h = current;
  current = next;
  next = h;
  
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
      if (current->cells[i][j]) {
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
      c += current->cells[i][j];
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

  current = calloc(1, sizeof(Buffer));
  next = calloc(1, sizeof(Buffer));

  current_worklist = calloc(1, sizeof(Worklist));
  next_worklist = calloc(1, sizeof(Worklist));

  for (Celllist* l = gen0; l; l = l->next){
    char* field = &current->cells[l->x + OFFSET][l->y + OFFSET];
    *field = 1;
    push(current_worklist, field);
  }
  freecelllist(gen0);

  for (i=0; i<generations; i++) {
    onegeneration();
  }
  writelife(stdout);
  fprintf(stderr,"%ld cells alive\n", countcells());
  return 0;
}
