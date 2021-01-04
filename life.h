#include <stdio.h>

#define BUFFER_SIZE 2300
#define BUCKET_COUNT 200
#define BUCKET_SIZE 20
// this is to support negative values
#define OFFSET 200

typedef struct celllist {
  long x, y;
  struct celllist *next;
} Celllist;

typedef struct worklistbucket {
  char* elements[BUCKET_COUNT];
  int current;
} WorklistBucket;

typedef struct worklist {
  WorklistBucket* buckets[BUCKET_COUNT];
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
