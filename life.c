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

void push(Worklist* worklist, char* field) {
  int bucket_index = ((long) field) % BUCKET_COUNT;
  WorklistBucket* bucket = worklist->buckets[bucket_index];

  for (int i = 0; i < bucket->current; i++) {
    if (bucket->elements[i] == field) {
      return;
    }
  }

  bucket->current++;
  bucket->elements[bucket->current] = field;
}

char* pop(Worklist* worklist) {
  for (int i = 0; i < BUCKET_COUNT; i++) {
    WorklistBucket* bucket = worklist->buckets[i];
    if (bucket->current >= 0) {
      return bucket->elements[bucket->current--];
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

  current_worklist = malloc(sizeof(Worklist));
  next_worklist = malloc(sizeof(Worklist));
  for (int i = 0; i < BUCKET_COUNT; i++) {
    current_worklist->buckets[i] = malloc(sizeof(WorklistBucket));
    next_worklist->buckets[i] = malloc(sizeof(WorklistBucket));
    current_worklist->buckets[i]->current = -1;
    next_worklist->buckets[i]->current = -1;
  }
  
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
