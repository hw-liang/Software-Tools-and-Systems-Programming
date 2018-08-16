#!/usr/bin/env bash

./echo_arg CSC209 > echo_out.txt
./echo_stdin < echo_stdin.c
count 210 > count.txt && wc -m < count.txt
ls -S | head -1 > largest.txt && ./echo_stdin < largest.txt
 
