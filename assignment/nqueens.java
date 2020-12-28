// derived from Niklaus Wirth's Pascal program (therefore not idiomatic Java)
// from "Algorithms + Data Structures = Programs", Prentice-Hall 1976 
// modified to produce and count all solutions

// Problem: Zaehle die Loesungen fuer folgendes Problem: Setze N Damen
// auf einem NxN-Schachbrett so, dass keine Dame eine andere schlagen
// kann.

public class nqueens {
    static int MAX_N = 100;
    static int n;

    static boolean[] a = new boolean[MAX_N + 1];   // 1..n

    // a[x] ist true, wenn man in Spalte x eine Dame setzen kann
    static boolean[] b = new boolean[2 * MAX_N + 1]; // 2..2*n

    // b[x+y] ist true, wenn man in der Diagonale,
    // die die Felder (x+k, y-k) (fuer alle k) enthaelt, eine Dame setzen kann
    static boolean[] c = new boolean[2 * MAX_N];   // 1..2*n-1

    // c[x-y+n] ist true, wenn man in der Diagonale,
    // die die Felder (x+k, y+k) (fuer alle k) enthaelt, eine Dame setzen kann
    static int[] x = new int[MAX_N + 1];    // 1..n

    // x[y] enthaelt die Spalten der aktuellen (Teil)loesung
    static int i;

    static long count = 0;

    public static void try1(int i) {
        int j = 0;
        do {
            j = j + 1;
            if (a[j] && b[i + j] && c[i - j + n]) {
                x[i] = j;
                a[j] = false;
                b[i + j] = false;
                c[i - j + n] = false;
                if (i < n) {
                    try1(i + 1);
                }
                else {
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

    public static void main(String[] args) {
        n = Integer.parseInt(args[0]);

        for (i = 1; i <= n; i++)
            a[i] = true;
        for (i = 2; i <= 2 * n; i++)
            b[i] = true;
        for (i = 1; i <= 2 * n - 1; i++)
            c[i] = true;
        try1(1);
        System.out.println(count);
    }
}
