#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "life.h"

const char BM_0010_0000 = 32;
const char BM_0000_0001 = 1;

FILE *infile;
Celllist *gen0;

Buffer* current;
Buffer* next;

size_t processed_index = 0;
char* processed[WORKLIST_SIZE * 9];

Worklist* current_worklist;
Worklist* next_worklist;

bool alive(char* cell) {
  *cell = 1;
  return true;
}

bool dead(char* cell) {
  *cell = 0;
  return true;
}

bool kill(char* cell) {
  *cell = 0;
  push(next_worklist, cell);
  return false;
}

bool resurrect(char* cell) {
  *cell = 1;
  push(next_worklist, cell);
  return false;
}

bool (*dispatch[18])(char*) = {
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

int inline neighbourhood(char* cell)
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
  return n & (BM_0010_0000 - 1);
}

void inline mark_as_processed(char* field) {
  *field |= BM_0010_0000;
}

void inline handle_cell(char* cell) {
  if ((*cell & BM_0010_0000) == 0) {
    char* cell_new = ((char*) next->cells) + (cell - ((char*) current->cells));
    long n = neighbourhood(cell);
    bool needs_deletion = dispatch[(size_t) (n << 1) | *cell](cell_new);
    mark_as_processed(cell);
    if (needs_deletion) {
      processed[processed_index++] = cell;
    }
  }
}

void onegeneration()
{
  for (int i = 0; i < WORKLIST_SIZE; i++)
  {
    char* center = current_worklist->elements[i];
    if (center != NULL) {
      handle_cell(center - BUFFER_SIZE - 1);
      handle_cell(center - 1);
      handle_cell(center + BUFFER_SIZE - 1);
      handle_cell(center - BUFFER_SIZE);
      handle_cell(center);
      handle_cell(center + BUFFER_SIZE);
      handle_cell(center - BUFFER_SIZE + 1);
      handle_cell(center + 1);
      handle_cell(center + BUFFER_SIZE + 1);
    }
    current_worklist->elements[i] = NULL;
  }

  for (int i = 0; i < processed_index; i++)
  {
    *processed[i] &= BM_0000_0001;
  }
  
  processed_index = 0;

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
      if (current->cells[i][j] & BM_0000_0001) {
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
      if (current->cells[i][j] & BM_0000_0001) {
        c += 1;
      }
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
