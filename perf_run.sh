#!/bin/sh
perf stat -e cycles -e instructions -e cache-misses -e branch-misses ./nqueens 16 2>perf_result.txt 1>perf_output.txt