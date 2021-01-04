[Task Description](https://www.complang.tuwien.ac.at/anton/lvas/effizienz-aufgabe07/)

How to measure perfomance:

* `make perf` creates the result.txt files
* `perf record ./life 3000 < ./assignment/f0.l`
 * Samples data from the program
 * creates perf.data file
* `perf report` shows a report about the gathered data (perf.data)
* `perf annotate -d life` shows source code annotations