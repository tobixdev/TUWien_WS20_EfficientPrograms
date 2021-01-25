#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "life.h"

#define uchar unsigned char

const uchar BM_0100_0000 = 64;
const uchar BM_0000_0001 = 1;
const uchar BM_0011_1101 = 61;

FILE *infile;
Celllist *gen0;

Buffer* buffer;

Worklist* current_worklist;
Worklist* next_worklist;

void inline inc(uchar* cell) {
  *cell += 4;
  // if (((*cell & BM_0011_1101) >> 2) > 8) {
  //   int x = (cell - ((uchar*) buffer->cells)) / BUFFER_SIZE;
  //   int y =(cell - ((uchar*) buffer->cells)) % BUFFER_SIZE;
  //   printf("Wrong inc %d,%d.\n", x - OFFSET, y - OFFSET);
  //   for(int i = -1; i <= 1; i++) {
  //     for(int j = -1; j <= 1; j++) {
  //       printf("%d", buffer->cells[x + i][y + j] & 1);
  //     }
  //     printf("\n");
  //   }
  // }
}

void inline dec(uchar* cell) {
  *cell -= 4;
}

void kill(uchar* cell) {
  dec(cell - BUFFER_SIZE - 1);
  dec(cell - 1);
  dec(cell + BUFFER_SIZE - 1);
  dec(cell - BUFFER_SIZE);
  dec(cell + BUFFER_SIZE);
  dec(cell - BUFFER_SIZE + 1);
  dec(cell + 1);
  dec(cell + BUFFER_SIZE + 1);
  push(next_worklist, cell);
}

void resurrect(uchar* cell) {
  inc(cell - BUFFER_SIZE - 1);
  inc(cell - 1);
  inc(cell + BUFFER_SIZE - 1);
  inc(cell - BUFFER_SIZE);
  inc(cell + BUFFER_SIZE);
  inc(cell - BUFFER_SIZE + 1);
  inc(cell + 1);
  inc(cell + BUFFER_SIZE + 1);
  push(next_worklist, cell);
}

void nop(uchar* cell) {
}

void unreachable(uchar* cell) {
  printf("Unreachable dispatch entry! (%d)", *cell);
  fflush(stdout);
  exit(1);
}

void (*dispatch[256])(uchar*) = {
  unreachable, // (00) +  0   0000 0000            0 neighbours
  unreachable, // (01) +  0   0000 0001
  unreachable, // (02) +  0   0000 0010
  unreachable, // (03) +  0   0000 0011
  unreachable, // (04) +  0   0000 0100            1 neighbour
  unreachable, // (05) +  0   0000 0101
  unreachable, // (06) +  0   0000 0110
  unreachable, // (07) +  0   0000 0111
  unreachable, // (08) +  0   0000 1000            2 neighbours
  unreachable, // (09) +  0   0000 1001
  unreachable, // (10) +  0   0000 1010
  unreachable, // (11) +  0   0000 1011
  unreachable, // (12) +  0   0000 1100            3 neighbours
  unreachable, // (13) +  0   0000 1101
  unreachable, // (14) +  0   0000 1110
  unreachable, // (15) +  0   0000 1111
  unreachable, // (16) +  0   0001 0000            4 neighbours
  unreachable, // (17) +  0   0001 0001
  unreachable, // (18) +  0   0001 0010
  unreachable, // (19) +  0   0001 0011
  unreachable, // (20) +  0   0001 0100            5 neighbours
  unreachable, // (21) +  0   0001 0101
  unreachable, // (22) +  0   0001 0110
  unreachable, // (23) +  0   0001 0111
  unreachable, // (24) +  0   0001 1000            6 neighbours
  unreachable, // (25) +  0   0001 1001
  unreachable, // (26) +  0   0001 1010
  unreachable, // (27) +  0   0001 1011
  unreachable, // (28) +  0   0001 1100            7 neighbours
  unreachable, // (29) +  0   0001 1101
  unreachable, // (30) +  0   0001 1110
  unreachable, // (31) +  0   0001 1111
  unreachable, // (32) +  0   0010 0000            8 neighbours
  unreachable, // (33) +  0   0010 0001
  unreachable, // (34) +  0   0010 0010
  unreachable, // (35) +  0   0010 0011
  unreachable, // (36) +  0   0010 0100    TOO MANY NEIGHBOURS
  unreachable, // (37) +  0   0010 0101    TOO MANY NEIGHBOURS
  unreachable, // (38) +  0   0010 0110    TOO MANY NEIGHBOURS
  unreachable, // (39) +  0   0010 0111    TOO MANY NEIGHBOURS
  unreachable, // (40) +  0   0010 1000    TOO MANY NEIGHBOURS
  unreachable, // (41) +  0   0010 1001    TOO MANY NEIGHBOURS
  unreachable, // (42) +  0   0010 1010    TOO MANY NEIGHBOURS
  unreachable, // (43) +  0   0010 1011    TOO MANY NEIGHBOURS
  unreachable, // (44) +  0   0010 1100    TOO MANY NEIGHBOURS
  unreachable, // (45) +  0   0010 1101    TOO MANY NEIGHBOURS
  unreachable, // (46) +  0   0010 1110    TOO MANY NEIGHBOURS
  unreachable, // (47) +  0   0010 1111    TOO MANY NEIGHBOURS
  unreachable, // (48) +  0   0011 0000    TOO MANY NEIGHBOURS
  unreachable, // (49) +  0   0011 0001    TOO MANY NEIGHBOURS
  unreachable, // (50) +  0   0011 0010    TOO MANY NEIGHBOURS
  unreachable, // (51) +  0   0011 0011    TOO MANY NEIGHBOURS
  unreachable, // (52) +  0   0011 0100    TOO MANY NEIGHBOURS
  unreachable, // (53) +  0   0011 0101    TOO MANY NEIGHBOURS
  unreachable, // (54) +  0   0011 0110    TOO MANY NEIGHBOURS
  unreachable, // (55) +  0   0011 0111    TOO MANY NEIGHBOURS
  unreachable, // (56) +  0   0011 1000    TOO MANY NEIGHBOURS
  unreachable, // (57) +  0   0011 1001    TOO MANY NEIGHBOURS
  unreachable, // (58) +  0   0011 1010    TOO MANY NEIGHBOURS
  unreachable, // (59) +  0   0011 1011    TOO MANY NEIGHBOURS
  unreachable, // (60) +  0   0011 1100    TOO MANY NEIGHBOURS
  unreachable, // (61) +  0   0011 1101    TOO MANY NEIGHBOURS
  unreachable, // (62) +  0   0011 1110    TOO MANY NEIGHBOURS
  unreachable, // (63) +  0   0011 1111    TOO MANY NEIGHBOURS
  unreachable, // (00) + 64   0100 0000
  unreachable, // (01) + 64   0100 0001
  unreachable, // (02) + 64   0100 0010
  unreachable, // (03) + 64   0100 0011
  unreachable, // (04) + 64   0100 0100
  unreachable, // (05) + 64   0100 0101
  unreachable, // (06) + 64   0100 0110
  unreachable, // (07) + 64   0100 0111
  unreachable, // (08) + 64   0100 1000
  unreachable, // (09) + 64   0100 1001
  unreachable, // (10) + 64   0100 1010
  unreachable, // (11) + 64   0100 1011
  unreachable, // (12) + 64   0100 1100
  unreachable, // (13) + 64   0100 1101
  unreachable, // (14) + 64   0100 1110
  unreachable, // (15) + 64   0100 1111
  unreachable, // (16) + 64   0101 0000
  unreachable, // (17) + 64   0101 0001
  unreachable, // (18) + 64   0101 0010
  unreachable, // (19) + 64   0101 0011
  unreachable, // (20) + 64   0101 0100
  unreachable, // (21) + 64   0101 0101
  unreachable, // (22) + 64   0101 0110
  unreachable, // (23) + 64   0101 0111
  unreachable, // (24) + 64   0101 1000
  unreachable, // (25) + 64   0101 1001
  unreachable, // (26) + 64   0101 1010
  unreachable, // (27) + 64   0101 1011
  unreachable, // (28) + 64   0101 1100
  unreachable, // (29) + 64   0101 1101
  unreachable, // (30) + 64   0101 1110
  unreachable, // (31) + 64   0101 1111
  unreachable, // (32) + 64   0110 0000
  unreachable, // (33) + 64   0110 0001
  unreachable, // (34) + 64   0110 0010
  unreachable, // (35) + 64   0110 0011
  unreachable, // (36) + 64   0110 0100    TOO MANY NEIGHBOURS
  unreachable, // (37) + 64   0110 0101    TOO MANY NEIGHBOURS
  unreachable, // (38) + 64   0110 0110    TOO MANY NEIGHBOURS
  unreachable, // (39) + 64   0110 0111    TOO MANY NEIGHBOURS
  unreachable, // (40) + 64   0110 1000    TOO MANY NEIGHBOURS
  unreachable, // (41) + 64   0110 1001    TOO MANY NEIGHBOURS
  unreachable, // (42) + 64   0110 1010    TOO MANY NEIGHBOURS
  unreachable, // (43) + 64   0110 1011    TOO MANY NEIGHBOURS
  unreachable, // (44) + 64   0110 1100    TOO MANY NEIGHBOURS
  unreachable, // (45) + 64   0110 1101    TOO MANY NEIGHBOURS
  unreachable, // (46) + 64   0110 1110    TOO MANY NEIGHBOURS
  unreachable, // (47) + 64   0110 1111    TOO MANY NEIGHBOURS
  unreachable, // (48) + 64   0111 0000    TOO MANY NEIGHBOURS
  unreachable, // (49) + 64   0111 0001    TOO MANY NEIGHBOURS
  unreachable, // (50) + 64   0111 0010    TOO MANY NEIGHBOURS
  unreachable, // (51) + 64   0111 0011    TOO MANY NEIGHBOURS
  unreachable, // (52) + 64   0111 0100    TOO MANY NEIGHBOURS
  unreachable, // (53) + 64   0111 0101    TOO MANY NEIGHBOURS
  unreachable, // (54) + 64   0111 0110    TOO MANY NEIGHBOURS
  unreachable, // (55) + 64   0111 0111    TOO MANY NEIGHBOURS
  unreachable, // (56) + 64   0111 1000    TOO MANY NEIGHBOURS
  unreachable, // (57) + 64   0111 1001    TOO MANY NEIGHBOURS
  unreachable, // (58) + 64   0111 1010    TOO MANY NEIGHBOURS
  unreachable, // (59) + 64   0111 1011    TOO MANY NEIGHBOURS
  unreachable, // (60) + 64   0111 1100    TOO MANY NEIGHBOURS
  unreachable, // (61) + 64   0111 1101    TOO MANY NEIGHBOURS
  unreachable, // (62) + 64   0111 1110    TOO MANY NEIGHBOURS
  unreachable, // (63) + 64   0111 1111    TOO MANY NEIGHBOURS
  nop,         // (00) + 128  1000 0000            0 neighbours
  resurrect,   // (01) + 128  1000 0001
  kill,        // (02) + 128  1000 0010
  nop,         // (03) + 128  1000 0011
  nop,         // (04) + 128  1000 0100            1 neighbour
  resurrect,   // (05) + 128  1000 0101
  kill,        // (06) + 128  1000 0110
  nop,         // (07) + 128  1000 0111
  nop,         // (08) + 128  1000 1000            2 neighbours
  resurrect,   // (09) + 128  1000 1001
  kill,        // (10) + 128  1000 1010
  nop,         // (11) + 128  1000 1011
  nop,         // (12) + 128  1000 1100            3 neighbours
  resurrect,   // (13) + 128  1000 1101
  kill,        // (14) + 128  1000 1110
  nop,         // (15) + 128  1000 1111
  nop,         // (16) + 128  1001 0000            4 neighbours
  resurrect,   // (17) + 128  1001 0001
  kill,        // (18) + 128  1001 0010
  nop,         // (19) + 128  1001 0011
  nop,         // (20) + 128  1001 0100            5 neighbours
  resurrect,   // (21) + 128  1001 0101
  kill,        // (22) + 128  1001 0110
  nop,         // (23) + 128  1001 0111
  nop,         // (24) + 128  1001 1000            6 neighbours
  resurrect,   // (25) + 128  1001 1001
  kill,        // (26) + 128  1001 1010
  nop,         // (27) + 128  1001 1011
  nop,         // (28) + 128  1001 1100            7 neighbours
  resurrect,   // (29) + 128  1001 1101
  kill,        // (30) + 128  1001 1110
  nop,         // (31) + 128  1001 1111
  nop,         // (32) + 128  1010 0000            8 neighbours
  resurrect,   // (33) + 128  1010 0001
  kill,        // (34) + 128  1010 0010
  nop,         // (35) + 128  1010 0011
  unreachable, // (36) + 128  1010 0100    TOO MANY NEIGHBOURS
  unreachable, // (37) + 128  1010 0101    TOO MANY NEIGHBOURS
  unreachable, // (38) + 128  1010 0110    TOO MANY NEIGHBOURS
  unreachable, // (39) + 128  1010 0111    TOO MANY NEIGHBOURS
  unreachable, // (40) + 128  1010 1000    TOO MANY NEIGHBOURS
  unreachable, // (41) + 128  1010 1001    TOO MANY NEIGHBOURS
  unreachable, // (42) + 128  1010 1010    TOO MANY NEIGHBOURS
  unreachable, // (43) + 128  1010 1011    TOO MANY NEIGHBOURS
  unreachable, // (44) + 128  1010 1100    TOO MANY NEIGHBOURS
  unreachable, // (45) + 128  1010 1101    TOO MANY NEIGHBOURS
  unreachable, // (46) + 128  1010 1110    TOO MANY NEIGHBOURS
  unreachable, // (47) + 128  1010 1111    TOO MANY NEIGHBOURS
  unreachable, // (48) + 128  1011 0000    TOO MANY NEIGHBOURS
  unreachable, // (49) + 128  1011 0001    TOO MANY NEIGHBOURS
  unreachable, // (50) + 128  1011 0010    TOO MANY NEIGHBOURS
  unreachable, // (51) + 128  1011 0011    TOO MANY NEIGHBOURS
  unreachable, // (52) + 128  1011 0100    TOO MANY NEIGHBOURS
  unreachable, // (53) + 128  1011 0101    TOO MANY NEIGHBOURS
  unreachable, // (54) + 128  1011 0110    TOO MANY NEIGHBOURS
  unreachable, // (55) + 128  1011 0111    TOO MANY NEIGHBOURS
  unreachable, // (56) + 128  1011 1000    TOO MANY NEIGHBOURS
  unreachable, // (57) + 128  1011 1001    TOO MANY NEIGHBOURS
  unreachable, // (58) + 128  1011 1010    TOO MANY NEIGHBOURS
  unreachable, // (59) + 128  1011 1011    TOO MANY NEIGHBOURS
  unreachable, // (60) + 128  1011 1100    TOO MANY NEIGHBOURS
  unreachable, // (61) + 128  1011 1101    TOO MANY NEIGHBOURS
  unreachable, // (62) + 128  1011 1110    TOO MANY NEIGHBOURS
  unreachable, // (63) + 128  1011 1111    TOO MANY NEIGHBOURS
  nop,         // (00) + 196  1100 0000
  nop,         // (01) + 196  1100 0001
  nop,         // (02) + 196  1100 0010
  nop,         // (03) + 196  1100 0011
  nop,         // (04) + 196  1100 0100
  nop,         // (05) + 196  1100 0101
  nop,         // (06) + 196  1100 0110
  nop,         // (07) + 196  1100 0111
  nop,         // (08) + 196  1100 1000
  nop,         // (09) + 196  1100 1001
  nop,         // (10) + 196  1100 1010
  nop,         // (11) + 196  1100 1011
  nop,         // (12) + 196  1100 1100
  nop,         // (13) + 196  1100 1101
  nop,         // (14) + 196  1100 1110
  nop,         // (15) + 196  1100 1111
  nop,         // (16) + 196  1101 0000
  nop,         // (17) + 196  1101 0001
  nop,         // (18) + 196  1101 0010
  nop,         // (19) + 196  1101 0011
  nop,         // (20) + 196  1101 0100
  nop,         // (21) + 196  1101 0101
  nop,         // (22) + 196  1101 0110
  nop,         // (23) + 196  1101 0111
  nop,         // (24) + 196  1101 1000
  nop,         // (25) + 196  1101 1001
  nop,         // (26) + 196  1101 1010
  nop,         // (27) + 196  1101 1011
  nop,         // (28) + 196  1101 1100
  nop,         // (29) + 196  1101 1101
  nop,         // (30) + 196  1101 1110
  nop,         // (31) + 196  1101 1111
  nop,         // (32) + 196  1110 0000
  nop,         // (33) + 196  1110 0001
  nop,         // (34) + 196  1110 0010
  nop,         // (35) + 196  1110 0011
  unreachable, // (36) + 196  1110 0100    TOO MANY NEIGHBOURS
  unreachable, // (37) + 196  1110 0101    TOO MANY NEIGHBOURS
  unreachable, // (38) + 196  1110 0110    TOO MANY NEIGHBOURS
  unreachable, // (39) + 196  1110 0111    TOO MANY NEIGHBOURS
  unreachable, // (40) + 196  1110 1000    TOO MANY NEIGHBOURS
  unreachable, // (41) + 196  1110 1001    TOO MANY NEIGHBOURS
  unreachable, // (42) + 196  1110 1010    TOO MANY NEIGHBOURS
  unreachable, // (43) + 196  1110 1011    TOO MANY NEIGHBOURS
  unreachable, // (44) + 196  1110 1100    TOO MANY NEIGHBOURS
  unreachable, // (45) + 196  1110 1101    TOO MANY NEIGHBOURS
  unreachable, // (46) + 196  1110 1110    TOO MANY NEIGHBOURS
  unreachable, // (47) + 196  1110 1111    TOO MANY NEIGHBOURS
  unreachable, // (48) + 196  1111 0000    TOO MANY NEIGHBOURS
  unreachable, // (49) + 196  1111 0001    TOO MANY NEIGHBOURS
  unreachable, // (50) + 196  1111 0010    TOO MANY NEIGHBOURS
  unreachable, // (51) + 196  1111 0011    TOO MANY NEIGHBOURS
  unreachable, // (52) + 196  1111 0100    TOO MANY NEIGHBOURS
  unreachable, // (53) + 196  1111 0101    TOO MANY NEIGHBOURS
  unreachable, // (54) + 196  1111 0110    TOO MANY NEIGHBOURS
  unreachable, // (55) + 196  1111 0111    TOO MANY NEIGHBOURS
  unreachable, // (56) + 196  1111 1000    TOO MANY NEIGHBOURS
  unreachable, // (57) + 196  1111 1001    TOO MANY NEIGHBOURS
  unreachable, // (58) + 196  1111 1010    TOO MANY NEIGHBOURS
  unreachable, // (59) + 196  1111 1011    TOO MANY NEIGHBOURS
  unreachable, // (60) + 196  1111 1100    TOO MANY NEIGHBOURS
  unreachable, // (61) + 196  1111 1101    TOO MANY NEIGHBOURS
  unreachable, // (62) + 196  1111 1110    TOO MANY NEIGHBOURS
  unreachable, // (63) + 196  1111 1111    TOO MANY NEIGHBOURS
};

uchar transfer[(size_t)256] = {
  128 + 0,  // (00) 0000 0000            0 neighbours
  128 + 2,  // (01) 0000 0001
  128 + 0,  // (02) 0000 0010
  128 + 2,  // (03) 0000 0011
  128 + 4,  // (04) 0000 0100            1 neighbour
  128 + 6,  // (05) 0000 0101
  128 + 4,  // (06) 0000 0110
  128 + 6,  // (07) 0000 0111
  128 + 8,  // (08) 0000 1000            2 neighbours
  128 + 11, // (09) 0000 1001
  128 + 8,  // (10) 0000 1010
  128 + 11, // (11) 0000 1011
  128 + 13, // (12) 0000 1100            3 neighbours
  128 + 15, // (13) 0000 1101
  128 + 13, // (14) 0000 1110
  128 + 15, // (15) 0000 1111
  128 + 16, // (16) 0001 0000            4 neighbours
  128 + 18, // (17) 0001 0001
  128 + 16, // (18) 0001 0010
  128 + 18, // (19) 0001 0011
  128 + 20, // (20) 0001 0100            5 neighbours
  128 + 22, // (21) 0001 0101
  128 + 20, // (22) 0001 0110
  128 + 22, // (23) 0001 0111
  128 + 24, // (24) 0001 1000            6 neighbours
  128 + 26, // (25) 0001 1001
  128 + 24, // (26) 0001 1010
  128 + 26, // (27) 0001 1011
  128 + 28, // (28) 0001 1100            7 neighbours
  128 + 30, // (29) 0001 1101
  128 + 28, // (30) 0001 1110
  128 + 30, // (31) 0001 1111
  128 + 32, // (32) 0010 0000            8 neighbours
  128 + 34, // (33) 0010 0001
  128 + 32, // (34) 0010 0010
  128 + 34, // (35) 0010 0011
       255, // (36)                      Should not be needed
       255, // (37)
       255, // (38)
       255, // (39)
       255, // (40)
       255, // (41)
       255, // (42)
       255, // (43)
       255, // (44)
       255, // (45)
       255, // (46)
       255, // (47)
       255, // (48)
       255, // (49)
       255, // (50)
       255, // (51)
       255, // (52)
       255, // (53)
       255, // (54)
       255, // (55)
       255, // (56)
       255, // (57)
       255, // (58)
       255, // (59)
       255, // (60)
       255, // (61)
       255, // (62)
       255, // (63)
  128 + 0,  // (00) +  64
  128 + 2,  // (01) +  64
  128 + 0,  // (02) +  64
  128 + 2,  // (03) +  64
  128 + 4,  // (04) +  64
  128 + 6,  // (05) +  64
  128 + 4,  // (06) +  64
  128 + 6,  // (07) +  64
  128 + 8,  // (08) +  64
  128 + 11, // (09) +  64
  128 + 8,  // (10) +  64
  128 + 11, // (11) +  64
  128 + 13, // (12) +  64
  128 + 15, // (13) +  64
  128 + 13, // (14) +  64
  128 + 15, // (15) +  64
  128 + 16, // (16) +  64
  128 + 18, // (17) +  64
  128 + 16, // (18) +  64
  128 + 18, // (19) +  64
  128 + 20, // (20) +  64
  128 + 22, // (21) +  64
  128 + 20, // (22) +  64
  128 + 22, // (23) +  64
  128 + 24, // (24) +  64
  128 + 26, // (25) +  64
  128 + 24, // (26) +  64
  128 + 26, // (27) +  64
  128 + 28, // (28) +  64
  128 + 30, // (29) +  64
  128 + 28, // (30) +  64
  128 + 30, // (31) +  64
  128 + 32, // (32) +  64
  128 + 34, // (33) +  64
  128 + 32, // (34) +  64
  128 + 34, // (35) +  64
       255, // (36) +  64
       255, // (37) +  64
       255, // (38) +  64
       255, // (39) +  64
       255, // (40) +  64
       255, // (41) +  64
       255, // (42) +  64
       255, // (43) +  64
       255, // (44) +  64
       255, // (45) +  64
       255, // (46) +  64
       255, // (47) +  64
       255, // (48) +  64
       255, // (49) +  64
       255, // (50) +  64
       255, // (51) +  64
       255, // (52) +  64
       255, // (53) +  64
       255, // (54) +  64
       255, // (55) +  64
       255, // (56) +  64
       255, // (57) +  64
       255, // (58) +  64
       255, // (59) +  64
       255, // (60) +  64
       255, // (61) +  64
       255, // (62) +  64
       255, // (63) +  64
  128 + 0 , // (00) + 128
  128 + 1 , // (01) + 128
  128 + 2 , // (02) + 128
  128 + 3 , // (03) + 128
  128 + 4 , // (04) + 128
  128 + 5 , // (05) + 128
  128 + 6 , // (06) + 128
  128 + 7 , // (07) + 128
  128 + 8 , // (08) + 128
  128 + 9 , // (09) + 128
  128 + 10, // (10) + 128
  128 + 11, // (11) + 128
  128 + 12, // (12) + 128
  128 + 13, // (13) + 128
  128 + 14, // (14) + 128
  128 + 15, // (15) + 128
  128 + 16, // (16) + 128
  128 + 17, // (17) + 128
  128 + 18, // (18) + 128
  128 + 19, // (19) + 128
  128 + 20, // (20) + 128
  128 + 21, // (21) + 128
  128 + 22, // (22) + 128
  128 + 23, // (23) + 128
  128 + 24, // (24) + 128
  128 + 25, // (25) + 128
  128 + 26, // (26) + 128
  128 + 27, // (27) + 128
  128 + 28, // (28) + 128
  128 + 29, // (29) + 128
  128 + 30, // (30) + 128
  128 + 31, // (31) + 128
  128 + 32, // (32) + 128
  128 + 33, // (33) + 128
  128 + 34, // (34) + 128
  128 + 35, // (35) + 128
       255, // (36) + 128
       255, // (37) + 128
       255, // (38) + 128
       255, // (39) + 128
       255, // (40) + 128
       255, // (41) + 128
       255, // (42) + 128
       255, // (43) + 128
       255, // (44) + 128
       255, // (45) + 128
       255, // (46) + 128
       255, // (47) + 128
       255, // (48) + 128
       255, // (49) + 128
       255, // (50) + 128
       255, // (51) + 128
       255, // (52) + 128
       255, // (53) + 128
       255, // (54) + 128
       255, // (55) + 128
       255, // (56) + 128
       255, // (57) + 128
       255, // (58) + 128
       255, // (59) + 128
       255, // (60) + 128
       255, // (61) + 128
       255, // (62) + 128
       255, // (63) + 128
  128 + 0,  // (00) +  64
  128 + 2,  // (01) +  64
  128 + 0,  // (02) +  64
  128 + 2,  // (03) +  64
  128 + 4,  // (04) +  64
  128 + 6,  // (05) +  64
  128 + 4,  // (06) +  64
  128 + 6,  // (07) +  64
  128 + 8,  // (08) +  64
  128 + 11, // (09) +  64
  128 + 8,  // (10) +  64
  128 + 11, // (11) +  64
  128 + 13, // (12) +  64
  128 + 15, // (13) +  64
  128 + 13, // (14) +  64
  128 + 15, // (15) +  64
  128 + 16, // (16) +  64
  128 + 18, // (17) +  64
  128 + 16, // (18) +  64
  128 + 18, // (19) +  64
  128 + 20, // (20) +  64
  128 + 22, // (21) +  64
  128 + 20, // (22) +  64
  128 + 22, // (23) +  64
  128 + 24, // (24) +  64
  128 + 26, // (25) +  64
  128 + 24, // (26) +  64
  128 + 26, // (27) +  64
  128 + 28, // (28) +  64
  128 + 30, // (29) +  64
  128 + 28, // (30) +  64
  128 + 30, // (31) +  64
  128 + 32, // (32) +  64
  128 + 34, // (33) +  64
  128 + 32, // (34) +  64
  128 + 34, // (35) +  64
       255, // (36) + 192
       255, // (37) + 192
       255, // (38) + 192
       255, // (39) + 192
       255, // (40) + 192
       255, // (41) + 192
       255, // (42) + 192
       255, // (43) + 192
       255, // (44) + 192
       255, // (45) + 192
       255, // (46) + 192
       255, // (47) + 192
       255, // (48) + 192
       255, // (49) + 192
       255, // (50) + 192
       255, // (51) + 192
       255, // (52) + 192
       255, // (53) + 192
       255, // (54) + 192
       255, // (55) + 192
       255, // (56) + 192
       255, // (57) + 192
       255, // (58) + 192
       255, // (59) + 192
       255, // (60) + 192
       255, // (61) + 192
       255, // (62) + 192
       255, // (63) + 192
};  

size_t inline hash(uchar* field) {
  // NOTE: WORKLIST_SIZE has to be a power of two
  return ((size_t)field) & (WORKLIST_SIZE - 1);
}

void push(Worklist* worklist, uchar* field) {
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

void inline mark_as_processed(uchar* field) {
  *field |= BM_0100_0000;
}

void inline handle_cell(uchar* cell) {
  // int x = (cell - ((uchar*) buffer->cells)) / BUFFER_SIZE - OFFSET;
  // int y =(cell - ((uchar*) buffer->cells)) % BUFFER_SIZE - OFFSET;
  // printf("dispatch %d on pos (%d, %d) (%d)\n", *cell - 128, x, y, *cell);
  // fflush(stdout);
  dispatch[(size_t) *cell](cell);
  mark_as_processed(cell);
}

void inline transfer_fn(uchar* cell) {
  // int x = (cell - ((uchar*) buffer->cells)) / BUFFER_SIZE - OFFSET;
  // int y =(cell - ((uchar*) buffer->cells)) % BUFFER_SIZE - OFFSET;
  // printf("transfer result %d -> %d on pos (%d, %d)\n", *cell, transfer[(size_t) (*cell)], x, y);
  *cell = transfer[(size_t) (*cell)];
}

// void check(uchar* cell) {
//   uchar n = 0;
//   n += *(cell - BUFFER_SIZE - 1) & BM_0000_0001;
//   n += *(cell - 1) & BM_0000_0001;
//   n += *(cell + BUFFER_SIZE - 1) & BM_0000_0001;
//   n += *(cell - BUFFER_SIZE) & BM_0000_0001;
//   n += *(cell + BUFFER_SIZE) & BM_0000_0001;
//   n += *(cell - BUFFER_SIZE + 1) & BM_0000_0001;
//   n += *(cell + 1) & BM_0000_0001;
//   n += *(cell + BUFFER_SIZE + 1) & BM_0000_0001;
//   int x = (cell - ((uchar*) buffer->cells)) / BUFFER_SIZE - OFFSET;
//   int y = (cell - ((uchar*) buffer->cells)) % BUFFER_SIZE - OFFSET;
//   if (n != (*cell & BM_0011_1101) >> 2) {
//     printf("Wrong number of neighbours on (%d, %d). Is: %d. Should be: %d.\n", x, y, n, (*cell & BM_0011_1101) >> 2);
//     for(int i = -1; i < 3; i++) {
//       for(int j = -1; j < 3; j++) {
//         printf("%d", buffer->cells[i + OFFSET][j + OFFSET] & 1);
//       }
//       printf("\n");
//     }
//     fflush(stdout);
//     exit(1);
//   } else if (n > 0) {
//     printf("(%d, %d, %d)\n", x, y, n);
//   }
// }

// void verify_board() {
//   for (int i = 1; i < BUFFER_SIZE - 1; i++) {
//     for (int j = 1; j < BUFFER_SIZE - 1; j++) {
//       check(&buffer->cells[i][j]);
//     }
//   }
//   for(int i = 1; i < 4; i++) {
//     for(int j = -2; j < 3; j++) {
//       printf("%d", buffer->cells[i + OFFSET][j + OFFSET] & 1);
//     }
//     printf("\n");
//   }
// }

void onegeneration()
{
  // verify_board();

  for (int i = 0; i < WORKLIST_SIZE; i++)
  {
    uchar* center = current_worklist->elements[i];
    if (center != NULL) {
      transfer_fn(center - BUFFER_SIZE - 1);
      transfer_fn(center - 1);
      transfer_fn(center + BUFFER_SIZE - 1);
      transfer_fn(center - BUFFER_SIZE);
      transfer_fn(center);
      transfer_fn(center + BUFFER_SIZE);
      transfer_fn(center - BUFFER_SIZE + 1);
      transfer_fn(center + 1);
      transfer_fn(center + BUFFER_SIZE + 1);
    }
  }

  for (int i = 0; i < WORKLIST_SIZE; i++)
  {
    uchar* center = current_worklist->elements[i];
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
      if (buffer->cells[i][j] & BM_0000_0001) {
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
      if (buffer->cells[i][j] & BM_0000_0001) {
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

  buffer = calloc(1, sizeof(Buffer));

  current_worklist = calloc(1, sizeof(Worklist));
  next_worklist = calloc(1, sizeof(Worklist));

  for (Celllist* l = gen0; l; l = l->next){
    uchar* cell = &buffer->cells[l->x + OFFSET][l->y + OFFSET];
    *cell |= 3;
    resurrect(cell);
  }
  freecelllist(gen0);

  // because resurrect pushes into next work_list
  Worklist* h = current_worklist;
  current_worklist = next_worklist;
  next_worklist = h;

  for (i=0; i<generations; i++) {
    onegeneration();
  }
  writelife(stdout);
  fprintf(stderr,"%ld cells alive\n", countcells());
  return 0;
}
