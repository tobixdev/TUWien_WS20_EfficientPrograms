#!/bin/sh
perf stat -e cycles -e instructions -e cache-misses -e branch-misses java nqueens 8 2>perf_result.txt 1>perf_output.txt