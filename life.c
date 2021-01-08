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


/**
 * Things to try:
 * - Remove branchless "optimization". (compare afterwards). Probably hurts more than it helps. 
 * - Play with additional information in cell. e.g. neighbour count.
 * -- Store if a cell is active or not active in a single byte -> no 2nd buffer
 * - Improve hash set.
 * -- https://www.sebastiansylvan.com/post/robin-hood-hashing-should-be-your-default-hash-table-implementation/
 * -- Hash Set Size adjustments
 * -- Modulo operation for hash -> Could be exchanged for a bitmask
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

void push(Worklist* worklist, char* field) {
  int probe_start = ((size_t) field) % WORKLIST_SIZE;

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

char* pop(Worklist* worklist) {
  for (int i = worklist->last_probed; i < WORKLIST_SIZE; i++)
  {
    char* element = worklist->elements[i];
    if (element != NULL) {
      worklist->elements[i] = NULL;
      worklist->last_probed = i + 1;
      return element;
    }
  }
  return NULL;
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
  char* cell = NULL;
  while((cell = pop(current_worklist)) != NULL) {
      long n = neighbourhood(cell);
      char* cell_new = ((char*) next->cells) + (cell - ((char*) current->cells));
      *cell_new = (n == 3) | (n == 11) | (n == 12);
      if ((*cell_new) != (*cell)) {
        push_neighbourhood(next_worklist, cell_new);
      }
  }
  Buffer* h = current;
  current = next;
  next = h;
  
  Worklist* h_worklist = current_worklist;
  current_worklist = next_worklist;
  next_worklist = h_worklist;
  next_worklist->last_probed = 0;
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
    current->cells[l->x + OFFSET][l->y + OFFSET] = 1;
    push_neighbourhood(current_worklist, &current->cells[l->x + OFFSET][l->y + OFFSET]);
  }
  freecelllist(gen0);

  for (i=0; i<generations; i++) {
    onegeneration();
  }
  writelife(stdout);
  fprintf(stderr,"%ld cells alive\n", countcells());
  return 0;
}
