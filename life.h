#include <stdio.h>

#define BUFFER_SIZE 2300
#define WORK_LIST_SIZE 100000
// this is to support negative values
#define OFFSET 200

typedef struct celllist {
  long x, y;
  struct celllist *next;
} Celllist;

typedef struct worklist {
  char* elements[WORK_LIST_SIZE];
  int current;
} Worklist;

void push(Worklist* worklist, char* field);
char* pop(Worklist* worklist);

extern Celllist *gen0;
extern FILE *infile;

int yyparse(void);
Celllist *newcell(long x, long y, Celllist *l);
