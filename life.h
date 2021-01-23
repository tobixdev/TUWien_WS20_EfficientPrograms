#include <stdio.h>

#define BUFFER_SIZE 2300
#define WORKLIST_SIZE 32768
// this is to support negative values
#define OFFSET 200

typedef struct celllist {
  long x, y;
  struct celllist *next;
} Celllist;

typedef struct worklist {
  char* elements[WORKLIST_SIZE];
  int last_probed;
} Worklist;

typedef struct buffer {
  char cells[BUFFER_SIZE][BUFFER_SIZE];
} Buffer;

void push(Worklist* worklist, char* field);
char* pop(Worklist* worklist);

extern Celllist *gen0;
extern FILE *infile;

int yyparse(void);
Celllist *newcell(long x, long y, Celllist *l);
