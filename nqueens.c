#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#define MAX_N 100

static int n;

static bool a[MAX_N + 1];	// 1..n
// a[x] ist true, wenn man in Spalte x eine Dame setzen kann
static bool b[2 *MAX_N + 1];	// 2..2*n
// b[x+y] ist true, wenn man in der Diagonale, 
// die die Felder (x+k, y-k) (fuer alle k) enthaelt, eine Dame setzen kann
static bool c[2 *MAX_N];	// 1..2*n-1
// c[x-y+n] ist true, wenn man in der Diagonale,
// die die Felder (x+k, y+k) (fuer alle k) enthaelt, eine Dame setzen kann
static int x[MAX_N + 1];	// 1..n
// x[y] enthaelt die Spalten der aktuellen (Teil)loesung
static int i;
static long count = 0;

static void set_board_size(char *sizeString)
{
    char *endptr;

    errno = 0;	// To distinguish success/failure after call
    n = strtol(sizeString, &endptr, 10);

   	// Check for various possible errors
    if ((errno == ERANGE && (n == LONG_MAX || n == LONG_MIN)) || (errno != 0 && n == 0))
    {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (endptr == sizeString)
    {
        fprintf(stderr, "No digits were found\n");
        exit(EXIT_FAILURE);
    }

    if (*endptr != '\0')
    {
        fprintf(stderr, "Invalid characters after number pattern: %s\n", endptr);
        exit(EXIT_FAILURE);
    }

   	// If we got here, strtol() successfully parsed the input argument as board size
}

static void try1(int i)
{
    int j = 0;
    do {
        j = j + 1;
        if (a[j] && b[i + j] && c[i - j + n])
        {
            x[i] = j;
            a[j] = false;
            b[i + j] = false;
            c[i - j + n] = false;
            if (i < n)
            {
                try1(i + 1);
            }
            else
            {
               	// for (int k=1; k<=n; k++)
               	// 	System.out.print(x[k]);
               	// System.out.println();
                count++;
            }

            a[j] = true;
            b[i + j] = true;
            c[i - j + n] = true;
        }
    } while (j != n);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s n\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    set_board_size(argv[1]);

    for (i=1; i<=n; i++)
	    a[i]=true;
	for (i=2; i<=2*n; i++)
	    b[i]=true;
	for (i=1; i<=2*n-1; i++)
	    c[i]=true;
	try1(1);

    printf("%ld\n", count);

    exit(EXIT_SUCCESS);
}